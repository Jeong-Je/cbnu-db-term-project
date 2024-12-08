#include <stdlib.h>
#include <iostream>
#include <string>
#include "mysql_connection.h"
#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/prepared_statement.h>
#include <cppconn/resultset.h>

using namespace std;

const string server = ""; // MySQL 서버 주소
const string username = "";               // 사용자 이름
const string password = "";                   // 사용자 비밀번호

// 메뉴 출력 함수
void showMenu() {
    cout << "1. View Club List" << endl;
    cout << "2. View Club Members and Details" << endl;
    cout << "3. Add New Club" << endl;
    cout << "4. Search Club by Name" << endl;
    cout << "5. Exit" << endl;
    cout << "Choose an option: ";
}

// 동아리 목록을 출력하는 함수
void showClubs(sql::Connection* con) {
    sql::PreparedStatement* pstmt;
    sql::ResultSet* res;

    pstmt = con->prepareStatement("SELECT club_id, club_name FROM Club");
    res = pstmt->executeQuery();

    cout << "\nClub List:" << endl;
    while (res->next()) {
        // 각 동아리의 ID와 이름 출력
        cout << "Club ID: " << res->getInt(1)
            << ", Club Name: " << res->getString(2) << endl;
    }

    delete res;
    delete pstmt;
}

// 특정 동아리의 부원, 지도교수 및 활동 내역을 출력하는 함수
void showClubDetails(sql::Connection* con) {
    int club_id;
    cout << "Enter the Club ID to view details: ";
    cin >> club_id;

    // 동아리 정보 조회
    sql::PreparedStatement* pstmt;
    sql::ResultSet* res;

    // 동아리 부원 수 및 부원 목록 출력
    pstmt = con->prepareStatement(
        "SELECT COUNT(*) FROM Student WHERE club_id = ?");
    pstmt->setInt(1, club_id);
    res = pstmt->executeQuery();
    res->next();
    int memberCount = res->getInt(1);
    cout << "Number of Members: " << memberCount << endl;

    // 동아리 부원 목록 출력
    pstmt = con->prepareStatement(
        "SELECT name FROM Student WHERE club_id = ?");
    pstmt->setInt(1, club_id);
    res = pstmt->executeQuery();

    cout << "Members:" << endl;
    while (res->next()) {
        cout << "- " << res->getString(1) << endl;
    }

    // 동아리 지도교수 정보 출력
    pstmt = con->prepareStatement(
        "SELECT a.name FROM Advisor a JOIN Club c ON a.advisor_id = c.advisor_id WHERE c.club_id = ?");
    pstmt->setInt(1, club_id);
    res = pstmt->executeQuery();
    res->next();
    cout << "Advisor: " << res->getString(1) << endl;

    // 동아리 활동 내역 출력
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

// 새 동아리를 추가하는 함수
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

    // PreparedStatement를 사용하여 새 동아리를 데이터베이스에 추가
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

// 동아리명을 검색하는 함수
void searchClub(sql::Connection* con) {
    string searchTerm;
    cout << "Enter the club name to search: ";
    cin.ignore(); // 입력 버퍼 비우기
    getline(cin, searchTerm);

    sql::PreparedStatement* pstmt;
    sql::ResultSet* res;

    // 입력한 동아리명으로 검색
    pstmt = con->prepareStatement("SELECT * FROM Club WHERE club_name LIKE ?");
    pstmt->setString(1, "%" + searchTerm + "%");

    res = pstmt->executeQuery();

    cout << "\nSearch Results:" << endl;
    bool found = false;
    while (res->next()) {
        // 검색된 동아리 목록 출력
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
    // 콘솔의 문자셋을 UTF-8로 변경
    system("chcp 65001"); // 이거 안쓰면 한글이 깨짐

    sql::Driver* driver;
    sql::Connection* con;

    try {
        // 드라이버 인스턴스 가져오기
        driver = get_driver_instance();

        // 서버에 연결
        con = driver->connect(server, username, password);

        // 데이터베이스 선택
        con->setSchema("");

        int choice;
        do {
            showMenu(); // 메뉴 출력
            cin >> choice;

            switch (choice) {
            case 1:
                showClubs(con);  // 동아리 목록 보기
                break;
            case 2:
                showClubDetails(con); // 특정 동아리 정보 보기
                break;
            case 3:
                addClub(con);    // 새 동아리 추가
                break;
            case 4:
                searchClub(con); // 동아리 검색
                break;
            case 5:
                cout << "Exiting the program." << endl; // 프로그램 종료 메시지
                break;
            default:
                cout << "Invalid input. Please try again." << endl; // 잘못된 입력 처리
            }

        } while (choice != 5); // 종료 전까지 반복

        delete con; // 연결 종료
    }
    catch (sql::SQLException& e) {
        cout << "Could not connect to server. Error message: " << e.what() << endl;
        system("pause");
        exit(1);
    }

    system("pause");
    return 0;
}