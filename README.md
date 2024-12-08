# CBNU 데이터베이스시스템 학기 프로젝트 
소프트웨어학부 동아리 관리시스템 시스템을 구축하라.

## MySQL 테이블 구조

### 동아리 테이블 (Club)

| 컬럼명       | 데이터 타입         | 제약조건                                       |
|--------------|---------------------|------------------------------------------------|
| club_id      | INT                 | AUTO_INCREMENT, PRIMARY KEY                    |
| club_name    | VARCHAR(100)        | NOT NULL                                       |
| advisor_id   | INT                 | FOREIGN KEY (advisor_id) REFERENCES Advisor(advisor_id) |
| website_url  | VARCHAR(255)        |                                                |

### 지도교수 테이블 (Advisor)

| 컬럼명       | 데이터 타입         | 제약조건                                       |
|--------------|---------------------|------------------------------------------------|
| advisor_id   | INT                 | AUTO_INCREMENT, PRIMARY KEY                    |
| name         | VARCHAR(100)        | NOT NULL                                       |
| office       | VARCHAR(100)        |                                                |
| email        | VARCHAR(100)        | UNIQUE                                         |

### 학생 테이블 (Student)

| 컬럼명       | 데이터 타입         | 제약조건                                       |
|--------------|---------------------|------------------------------------------------|
| student_id   | INT                 | PRIMARY KEY                                   |
| name         | VARCHAR(100)        | NOT NULL                                       |
| sex          | ENUM('M', 'F')      | NOT NULL                                       |
| phone_number | VARCHAR(15)         |                                                |

### 학생-동아리 M:N 관계 테이블 (Student_Club)

| 컬럼명       | 데이터 타입         | 제약조건                                       |
|--------------|---------------------|------------------------------------------------|
| student_id   | INT                 | FOREIGN KEY (student_id) REFERENCES Student(student_id) |
| club_id      | INT                 | FOREIGN KEY (club_id) REFERENCES Club(club_id) |
| position     | VARCHAR(100)        |                                                |
| PRIMARY KEY  | (student_id, club_id)|                                                |

### 활동 성과 테이블 (Achievement)

| 컬럼명        | 데이터 타입        | 제약조건                                      |
|---------------|--------------------|-----------------------------------------------|
| achievement_id| INT                | AUTO_INCREMENT, PRIMARY KEY                   |
| description   | TEXT               | NOT NULL                                      |
| club_id       | INT                | FOREIGN KEY (club_id) REFERENCES Club(club_id) |
| achievement_date | DATE            |                                                |

### 학생-활동 성과 M:N 관계 테이블 (Student_Achievement)

| 컬럼명        | 데이터 타입        | 제약조건                                      |
|---------------|--------------------|-----------------------------------------------|
| student_id    | INT                | FOREIGN KEY (student_id) REFERENCES Student(student_id) |
| achievement_id| INT                | FOREIGN KEY (achievement_id) REFERENCES Achievement(achievement_id) |
| role          | VARCHAR(100)       |                                                |
| PRIMARY KEY   | (student_id, achievement_id) |                                    |
