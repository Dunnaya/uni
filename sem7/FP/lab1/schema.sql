SET NAMES utf8mb4;
DROP DATABASE IF EXISTS sport_faculty;
CREATE DATABASE sport_faculty CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci;
USE sport_faculty;

-- set DB_PASS
CREATE USER IF NOT EXISTS 'sport'@'%'         IDENTIFIED BY 'sport123';
CREATE USER IF NOT EXISTS 'sport'@'localhost' IDENTIFIED BY 'sport123';
GRANT ALL PRIVILEGES ON sport_faculty.* TO 'sport'@'%';
GRANT ALL PRIVILEGES ON sport_faculty.* TO 'sport'@'localhost';
FLUSH PRIVILEGES;

CREATE TABLE students (
  id        INT AUTO_INCREMENT PRIMARY KEY,
  full_name VARCHAR(100) NOT NULL,
  grp       VARCHAR(20)  NOT NULL DEFAULT '',
  phone     VARCHAR(20)  NOT NULL DEFAULT ''
);

CREATE TABLE teachers (
  id        INT AUTO_INCREMENT PRIMARY KEY,
  full_name VARCHAR(100) NOT NULL,
  position  VARCHAR(60)  NOT NULL DEFAULT '',
  phone     VARCHAR(20)  NOT NULL DEFAULT ''
);

CREATE TABLE sections (
  id       INT AUTO_INCREMENT PRIMARY KEY,
  name     VARCHAR(80) NOT NULL,
  coach_id INT NOT NULL,
  FOREIGN KEY (coach_id) REFERENCES teachers(id)
);

CREATE TABLE student_sections (
  student_id INT NOT NULL,
  section_id INT NOT NULL,
  PRIMARY KEY (student_id, section_id),
  FOREIGN KEY (student_id) REFERENCES students(id) ON DELETE CASCADE,
  FOREIGN KEY (section_id) REFERENCES sections(id) ON DELETE CASCADE
);

CREATE TABLE teacher_sections (
  teacher_id INT NOT NULL,
  section_id INT NOT NULL,
  PRIMARY KEY (teacher_id, section_id),
  FOREIGN KEY (teacher_id) REFERENCES teachers(id) ON DELETE CASCADE,
  FOREIGN KEY (section_id) REFERENCES sections(id) ON DELETE CASCADE
);

CREATE TABLE section_schedule (
  id         INT AUTO_INCREMENT PRIMARY KEY,
  section_id INT NOT NULL,
  weekday    VARCHAR(15) NOT NULL,
  start_time TIME NOT NULL,
  end_time   TIME NOT NULL,
  place      VARCHAR(80) NOT NULL DEFAULT '',
  FOREIGN KEY (section_id) REFERENCES sections(id) ON DELETE CASCADE
);

CREATE TABLE competitions (
  id         INT AUTO_INCREMENT PRIMARY KEY,
  title      VARCHAR(120) NOT NULL,
  section_id INT NOT NULL,
  comp_date  DATE NOT NULL,
  place      VARCHAR(80) NOT NULL DEFAULT '',
  FOREIGN KEY (section_id) REFERENCES sections(id) ON DELETE CASCADE
);

-- sample data
INSERT INTO students (full_name, grp, phone) VALUES
 ('Kovalenko Oleksandr', 'PM-21', '+380501112233'),
 ('Shevchenko Maria',    'PM-21', '+380502223344'),
 ('Bondarenko Andrii',   'PM-22', '+380503334455'),
 ('Melnyk Iryna',        'CS-31', '+380504445566'),
 ('Tkachenko Dmytro',    'CS-31', '+380505556677'),
 ('Kravchenko Oksana',   'PM-23', '+380506667788');

INSERT INTO teachers (full_name, position, phone) VALUES
 ('Petrenko Vasyl',   'Associate professor', '+380671112233'),
 ('Ivanenko Nataliia','Senior lecturer',     '+380672223344'),
 ('Hrytsenko Oleh',   'Professor',           '+380673334455');

INSERT INTO sections (name, coach_id) VALUES
 ('Volleyball',   1),
 ('Basketball',   3),
 ('Table tennis', 2),
 ('Chess',        3);

INSERT INTO student_sections VALUES
 (1,1),(2,1),(3,2),(4,3),(5,2),(6,3),(1,4),(5,4);

INSERT INTO teacher_sections VALUES
 (1,1),(2,3),(3,2),(3,4),(2,1);

INSERT INTO section_schedule (section_id, weekday, start_time, end_time, place) VALUES
 (1,'Tuesday',  '18:00','19:30','Gym 1'),
 (1,'Thursday', '18:00','19:30','Gym 1'),
 (2,'Wednesday','17:00','18:30','Gym 2'),
 (3,'Friday',   '15:00','16:30','Game room'),
 (4,'Monday',   '16:00','17:30','Room 215');

INSERT INTO competitions (title, section_id, comp_date, place) VALUES
 ('Faculty volleyball championship', 1, '2026-11-12', 'Gym 1'),
 ('Table tennis tournament',         3, '2026-11-26', 'Game room'),
 ('Faculty basketball cup',          2, '2026-12-05', 'Gym 2'),
 ('Chess tournament "Early Winter"', 4, '2026-12-14', 'Room 215'),
 ('Spring volleyball championship',  1, '2027-03-20', 'Gym 1');