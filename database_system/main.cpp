#include <stdlib.h>
#include <iostream>
#include <string>
#include "mysql_connection.h"
#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/prepared_statement.h>
#include <cppconn/resultset.h>

using namespace std;

const string server = ""; // MySQL ���� �ּ�
const string username = "";               // ����� �̸�
const string password = "";                   // ����� ��й�ȣ

// �޴� ��� �Լ�
void showMenu() {
    cout << "1. View Club List" << endl;
    cout << "2. View Club Members and Details" << endl;
    cout << "3. Add New Club" << endl;
    cout << "4. Search Club by Name" << endl;
    cout << "5. Exit" << endl;
    cout << "Choose an option: ";
}

// ���Ƹ� ����� ����ϴ� �Լ�
void showClubs(sql::Connection* con) {
    sql::PreparedStatement* pstmt;
    sql::ResultSet* res;

    pstmt = con->prepareStatement("SELECT club_id, club_name FROM Club");
    res = pstmt->executeQuery();

    cout << "\nClub List:" << endl;
    while (res->next()) {
        // �� ���Ƹ��� ID�� �̸� ���
        cout << "Club ID: " << res->getInt(1)
            << ", Club Name: " << res->getString(2) << endl;
    }

    delete res;
    delete pstmt;
}

// Ư�� ���Ƹ��� �ο�, �������� �� Ȱ�� ������ ����ϴ� �Լ�
void showClubDetails(sql::Connection* con) {
    int club_id;
    cout << "Enter the Club ID to view details: ";
    cin >> club_id;

    // ���Ƹ� ���� ��ȸ
    sql::PreparedStatement* pstmt;
    sql::ResultSet* res;

    // ���Ƹ� �ο� �� �� �ο� ��� ���
    pstmt = con->prepareStatement(
        "SELECT COUNT(*) FROM Student WHERE club_id = ?");
    pstmt->setInt(1, club_id);
    res = pstmt->executeQuery();
    res->next();
    int memberCount = res->getInt(1);
    cout << "Number of Members: " << memberCount << endl;

    // ���Ƹ� �ο� ��� ���
    pstmt = con->prepareStatement(
        "SELECT name FROM Student WHERE club_id = ?");
    pstmt->setInt(1, club_id);
    res = pstmt->executeQuery();

    cout << "Members:" << endl;
    while (res->next()) {
        cout << "- " << res->getString(1) << endl;
    }

    // ���Ƹ� �������� ���� ���
    pstmt = con->prepareStatement(
        "SELECT a.name FROM Advisor a JOIN Club c ON a.advisor_id = c.advisor_id WHERE c.club_id = ?");
    pstmt->setInt(1, club_id);
    res = pstmt->executeQuery();
    res->next();
    cout << "Advisor: " << res->getString(1) << endl;

    // ���Ƹ� Ȱ�� ���� ���
    pstmt = con->prepareStatement(
        "SELECT description FROM Achievement WHERE club_id = ?");
    pstmt->setInt(1, club_id);
    res = pstmt->executeQuery();

    cout << "Achievements:" << endl;
    bool found = false;
    while (res->next()) {
        cout << "- " << res->getString(1) << endl;
        found = true;
    }

    if (!found) {
        cout << "No achievements found for this club." << endl;
    }

    delete res;
    delete pstmt;
}

// �� ���Ƹ��� �߰��ϴ� �Լ�
void addClub(sql::Connection* con) {
    string clubname, website_url;
    int advisor_id;

    cout << "Enter the details for the new club." << endl;
    cout << "Club Name: ";
    cin.ignore();
    getline(cin, clubname);
    cout << "Advisor ID: ";
    cin >> advisor_id;
    cout << "Website URL: ";
    cin.ignore();
    getline(cin, website_url);

    // PreparedStatement�� ����Ͽ� �� ���Ƹ��� �����ͺ��̽��� �߰�
    sql::PreparedStatement* pstmt = con->prepareStatement(
        "INSERT INTO Club (club_name, advisor_id, website_url) VALUES (?, ?, ?)"
    );
    pstmt->setString(1, clubname);
    pstmt->setInt(2, advisor_id);
    pstmt->setString(3, website_url);

    pstmt->executeUpdate();

    cout << "New club has been added!" << endl;

    delete pstmt;
}

// ���Ƹ����� �˻��ϴ� �Լ�
void searchClub(sql::Connection* con) {
    string searchTerm;
    cout << "Enter the club name to search: ";
    cin.ignore(); // �Է� ���� ����
    getline(cin, searchTerm);

    sql::PreparedStatement* pstmt;
    sql::ResultSet* res;

    // �Է��� ���Ƹ������� �˻�
    pstmt = con->prepareStatement("SELECT * FROM Club WHERE club_name LIKE ?");
    pstmt->setString(1, "%" + searchTerm + "%");

    res = pstmt->executeQuery();

    cout << "\nSearch Results:" << endl;
    bool found = false;
    while (res->next()) {
        // �˻��� ���Ƹ� ��� ���
        cout << "Club ID: " << res->getInt(1)
            << ", Club Name: " << res->getString(2)
            << ", Advisor ID: " << res->getInt(3)
            << ", Website: " << res->getString(4) << endl;
        found = true;
    }

    if (!found) {
        cout << "No clubs found." << endl;
    }

    delete res;
    delete pstmt;
}

int main() {
    // �ܼ��� ���ڼ��� UTF-8�� ����
    system("chcp 65001"); // �̰� �Ⱦ��� �ѱ��� ����

    sql::Driver* driver;
    sql::Connection* con;

    try {
        // ����̹� �ν��Ͻ� ��������
        driver = get_driver_instance();

        // ������ ����
        con = driver->connect(server, username, password);

        // �����ͺ��̽� ����
        con->setSchema("");

        int choice;
        do {
            showMenu(); // �޴� ���
            cin >> choice;

            switch (choice) {
            case 1:
                showClubs(con);  // ���Ƹ� ��� ����
                break;
            case 2:
                showClubDetails(con); // Ư�� ���Ƹ� ���� ����
                break;
            case 3:
                addClub(con);    // �� ���Ƹ� �߰�
                break;
            case 4:
                searchClub(con); // ���Ƹ� �˻�
                break;
            case 5:
                cout << "Exiting the program." << endl; // ���α׷� ���� �޽���
                break;
            default:
                cout << "Invalid input. Please try again." << endl; // �߸��� �Է� ó��
            }

        } while (choice != 5); // ���� ������ �ݺ�

        delete con; // ���� ����
    }
    catch (sql::SQLException& e) {
        cout << "Could not connect to server. Error message: " << e.what() << endl;
        system("pause");
        exit(1);
    }

    system("pause");
    return 0;
}