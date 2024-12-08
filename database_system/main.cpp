#include <stdlib.h>
#include <iostream>
#include <string>
#include "mysql_connection.h"
#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/prepared_statement.h>
#include <cppconn/resultset.h>

using namespace std;

const string server = "tcp://192.168.56.101:4567"; // MySQL 서버 주소
const string username = "jeongje";               // 사용자 이름
const string password = "1234";                   // 사용자 비밀번호

// 메뉴 출력 함수
void showMenu() {
    wcout << "1. View Club List\n";
    cout << "2. View Club Members and Details\n";
    cout << "3. Add New Club\n";
    cout << "4. Search Club by Name\n";
    cout << "5. Delete Club by ID\n";
    cout << "6. Add Student to Club\n";
    cout << "7. Remove Member from Club\n"; 
    cout << "8. View Student Achievements\n";  
    cout << "9. Exit\n";
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
        cout << "Club ID: " << res->getInt(1)
            << ", Club Name: " << res->getString(2) << endl;
    }
    cout << "\n";
    delete res;
    delete pstmt;
}

// 특정 동아리의 부원, 지도교수 및 활동 내역을 출력하는 함수
void showClubDetails(sql::Connection* con) {
    int club_id;
    cout << "Enter the Club ID to view details: ";
    cin >> club_id;

    sql::PreparedStatement* pstmt;
    sql::ResultSet* res;

    // 동아리 부원 수 및 부원 목록 출력
    pstmt = con->prepareStatement(
        "SELECT COUNT(*) FROM Student_Club WHERE club_id = ?");
    pstmt->setInt(1, club_id);
    res = pstmt->executeQuery();
    res->next();
    int memberCount = res->getInt(1);
    cout << "Number of Members: " << memberCount << endl;

    // 동아리 부원 목록 출력
    pstmt = con->prepareStatement(
        "SELECT s.name, sc.position FROM Student s "
        "JOIN Student_Club sc ON s.student_id = sc.student_id "
        "WHERE sc.club_id = ?");
    pstmt->setInt(1, club_id);
    res = pstmt->executeQuery();

    cout << "Members:" << endl;
    while (res->next()) {
        cout << "- " << res->getString(1) << " (" << res->getString(2) << ")" << endl;
    }

    // 동아리 지도교수 정보 출력
    pstmt = con->prepareStatement(
        "SELECT a.name FROM Advisor a "
        "JOIN Club c ON a.advisor_id = c.advisor_id "
        "WHERE c.club_id = ?");
    pstmt->setInt(1, club_id);
    res = pstmt->executeQuery();
    res->next();
    cout << "Advisor: " << res->getString(1) << endl;

    // 동아리 활동 내역 출력
    pstmt = con->prepareStatement(
        "SELECT description, achievement_date FROM Achievement "
        "WHERE club_id = ?");
    pstmt->setInt(1, club_id);
    res = pstmt->executeQuery();

    cout << "Achievements:" << endl;
    bool found = false;
    while (res->next()) {
        // 날짜를 문자열로 가져온 후, 출력
        string achievementDate = res->getString(2);
        cout << "- " << res->getString(1) << " (Date: " << achievementDate << ")\n";
        found = true;
    }

    if (!found) {
        cout << "No achievements found for this club." << endl;
    }
    cout << "\n";
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

    sql::PreparedStatement* pstmt = con->prepareStatement(
        "INSERT INTO Club (club_name, advisor_id, website_url) VALUES (?, ?, ?)"
    );
    pstmt->setString(1, clubname);
    pstmt->setInt(2, advisor_id);
    pstmt->setString(3, website_url);

    pstmt->executeUpdate();

    cout << "new club has been added.\n";

    delete pstmt;
}

// 동아리명을 검색하는 함수
void searchClub(sql::Connection* con) {
    string searchTerm;
    cout << "Enter the club name to search: ";
    cin.ignore();
    getline(cin, searchTerm);

    sql::PreparedStatement* pstmt;
    sql::ResultSet* res;

    pstmt = con->prepareStatement("SELECT * FROM Club WHERE club_name LIKE ?");
    pstmt->setString(1, "%" + searchTerm + "%");

    res = pstmt->executeQuery();

    cout << "\nSearch Results:\n";
    bool found = false;
    while (res->next()) {
        cout << "Club ID: " << res->getInt(1)
            << ", Club Name: " << res->getString(2)
            << ", Advisor ID: " << res->getInt(3)
            << ", Website: " << res->getString(4) << endl;
        found = true;
    }

    if (!found) {
        cout << "not found";
    }
    cout << "\n";
    delete res;
    delete pstmt;
}

// 동아리 삭제하는 함수
void deleteClub(sql::Connection* con) {
    int club_id;
    cout << "Enter the Club ID to delete: ";
    cin >> club_id;

    sql::PreparedStatement* pstmt;

    // 먼저 해당 동아리의 활동 내역 삭제
    pstmt = con->prepareStatement("DELETE FROM Achievement WHERE club_id = ?");
    pstmt->setInt(1, club_id);
    pstmt->executeUpdate();
    delete pstmt;

    // 그 다음에 동아리 부원들의 club_id를 NULL로 업데이트
    pstmt = con->prepareStatement("DELETE FROM Student_Club WHERE club_id = ?");
    pstmt->setInt(1, club_id);
    pstmt->executeUpdate();
    delete pstmt;

    // 마지막으로 동아리 삭제
    pstmt = con->prepareStatement("DELETE FROM Club WHERE club_id = ?");
    pstmt->setInt(1, club_id);
    pstmt->executeUpdate();

    cout << club_id << "club has been deleted.\n";

    delete pstmt;
}

// 동아리에 부원 추가하는 함수
void addMemberToClub(sql::Connection* con) {
    int student_id, club_id;

    // 학생 ID와 동아리 ID 입력받기
    cout << "Enter the Student ID: ";
    cin >> student_id;
    cout << "Enter the Club ID: ";
    cin >> club_id;

    // Student_Club 테이블에 부원 추가
    sql::PreparedStatement* pstmt = con->prepareStatement(
        "INSERT INTO Student_Club (student_id, club_id) VALUES (?, ?)"
    );
    pstmt->setInt(1, student_id);
    pstmt->setInt(2, club_id);
    pstmt->executeUpdate();

    cout << "Student has been added to the club\n";

    delete pstmt;
}

// 특정 학생이 참여한 활동 내역을 조회하는 함수
void showStudentAchievements(sql::Connection* con) {
    int student_id;

    // 학생 ID 입력받기
    cout << "Enter the Student ID to view their achievements: ";
    cin >> student_id;

    // 학생이 속한 동아리의 활동 내역 조회
    sql::PreparedStatement* pstmt = con->prepareStatement(
        "SELECT a.description FROM Achievement a "
        "JOIN Club c ON a.club_id = c.club_id "
        "JOIN Student_Club sc ON c.club_id = sc.club_id "
        "WHERE sc.student_id = ?"
    );
    pstmt->setInt(1, student_id);

    sql::ResultSet* res = pstmt->executeQuery();

    cout << "\nAchievements for Student ID " << student_id << ":" << endl;
    bool found = false;
    while (res->next()) {
        cout << "- " << res->getString("description") << endl;
        found = true;
    }

    if (!found) {
        cout << "No achievements found for this student." << endl;
    }

    delete res;
    delete pstmt;
}


// 동아리에서 학생 탈퇴하는 함수
void removeMemberFromClub(sql::Connection* con) {
    int student_id, club_id;

    // 학생 ID와 동아리 ID 입력받기
    cout << "Enter the Student ID to remove from the club: ";
    cin >> student_id;
    cout << "Enter the Club ID: ";
    cin >> club_id;

    // Student_Club 테이블에서 해당 학생의 동아리 정보 삭제
    sql::PreparedStatement* pstmt = con->prepareStatement(
        "DELETE FROM Student_Club WHERE student_id = ? AND club_id = ?"
    );
    pstmt->setInt(1, student_id);
    pstmt->setInt(2, club_id);
    pstmt->executeUpdate();

    cout << "Student has been removed from the club!" << endl;

    delete pstmt;
}




int main() {
    // 콘솔의 문자셋을 UTF-8로 변경
    system("chcp 65001");

    sql::Driver* driver;
    sql::Connection* con;

    try {
        driver = get_driver_instance();
        con = driver->connect(server, username, password);

        con->setSchema("ClubManagement");

        int choice;
        do {
            showMenu();
            cin >> choice;

            switch (choice) {
            case 1:
                showClubs(con);
                break;
            case 2:
                showClubDetails(con);
                break;
            case 3:
                addClub(con);
                break;
            case 4:
                searchClub(con);
                break;
            case 5:
                deleteClub(con);
                break;
            case 6:
                addMemberToClub(con);
                break;
            case 7:
                removeMemberFromClub(con);
                break;
            case 8:
                showStudentAchievements(con);
                break;
            case 9:
                cout << "Exit" << endl;
                break;
            default:
                cout << "Wrong Argument" << endl;
            }

        } while (choice != 9);

        delete con;
    }
    catch (sql::SQLException& e) {
        cout << "Connect Failed. Error message: " << e.what() << endl;
        system("pause");
        exit(1);
    }

    system("pause");
    return 0;
}
