-- USED AI TO FILL THE TABLES --

-- I. Core Tables

-- 1. Roles
INSERT INTO "Roles" ("RoleName", "Description") VALUES
('Administrator', 'Full system access'),
('Instructor', 'Manages courses and grades'),
('Student', 'Views content and completes assignments');

-- 2. Users (5 Active, 1 Soft Deleted)
INSERT INTO "Users" ("RoleID", "FirstName", "LastName", "Email", "PasswordHash", "IsActive") VALUES
(1, 'Admin', 'System', 'admin@lms.com', 'hash_admin_123', TRUE),      -- UserID 1
(2, 'Ivan', 'Petrov', 'i.petrov@lms.com', 'hash_instr_1', TRUE),      -- UserID 2 (Instructor 1)
(2, 'Maria', 'Koval', 'm.koval@lms.com', 'hash_instr_2', TRUE),      -- UserID 3 (Instructor 2)
(3, 'Olena', 'Sydorenko', 'o.sydorenko@lms.com', 'hash_stud_1', TRUE),  -- UserID 4 (Student 1)
(3, 'Dmytro', 'Shevchenko', 'd.shevchenko@lms.com', 'hash_stud_2', TRUE), -- UserID 5 (Student 2)
(3, 'Anastasia', 'Melnyk', 'a.melnyk@lms.com', 'hash_stud_3', FALSE);  -- UserID 6 (Student 3 - Soft Deleted for testing)

-- 3. Instructors
INSERT INTO "Instructors" ("UserID", "Bio", "IsVerified") VALUES
(2, 'Ph.D. in Computer Science and Databases.', TRUE), -- InstructorID 1 (Ivan Petrov)
(3, 'Certified specialist in UI/UX Design.', TRUE);    -- InstructorID 2 (Maria Koval)

-- 4. Students
INSERT INTO "Students" ("UserID", "EnrollmentDate", "StudentStatus") VALUES
(4, CURRENT_DATE, 'Active'),    -- StudentID 1 (Olena)
(5, CURRENT_DATE, 'Active'),    -- StudentID 2 (Dmytro)
(6, CURRENT_DATE, 'On Hold');   -- StudentID 3 (Anastasia)

-- II. Courses and Content

-- 5. Categories
INSERT INTO "Categories" ("CategoryName") VALUES
('Programming'),
('Design'),
('Marketing');

-- 6. Courses
INSERT INTO "Courses" ("InstructorID", "CategoryID", "Title", "Description", "Price", "IsPublished", "IsDeleted") VALUES
(1, 1, 'SQL Fundamentals for Beginners', 'Detailed course on relational databases and SQL language.', 199.99, TRUE, FALSE),  -- CourseID 1
(1, 1, 'Advanced PostgreSQL and Optimization', 'Deep dive into complex queries and database tuning.', 299.99, FALSE, FALSE), -- CourseID 2 (Not Published)
(2, 2, 'Introduction to Figma and UI/UX', 'Creating prototypes and user interfaces.', 99.00, TRUE, FALSE), -- CourseID 3
(2, 3, 'SEO Optimization 2024', 'Techniques for search engine promotion.', 150.00, TRUE, FALSE);  -- CourseID 4

-- 7. Sections (for CourseID 1)
INSERT INTO "Sections" ("CourseID", "Title", "OrderNumber") VALUES
(1, 'Introduction to Databases', 1),   -- SectionID 1
(1, 'Basic DML Operators', 2),         -- SectionID 2
(1, 'Working with Relationships', 3);   -- SectionID 3

-- 8. Lessons (for SectionID 1 and 2)
INSERT INTO "Lessons" ("SectionID", "Title", "ContentType", "ContentURL", "Duration") VALUES
(1, 'What is the Relational Model', 'Video', 'url/video/1', 15), -- LessonID 1
(1, 'Keys and Indexes', 'Text', 'url/text/1', 10),    -- LessonID 2
(2, 'SELECT, FROM, WHERE clauses', 'Video', 'url/video/2', 20), -- LessonID 3
(2, 'INSERT, UPDATE, DELETE tasks', 'Quiz', 'url/quiz/1', 5);    -- LessonID 4

-- 9. Resources (for LessonID 3)
INSERT INTO "Resources" ("LessonID", "FileName", "FileType", "FileURL") VALUES
(3, 'SQL_DML_Cheatsheet', 'PDF', 'url/resource/dml.pdf'),
(3, 'Practical_Tasks', 'SQL', 'url/resource/tasks.sql');

-- III. Enrollment and Grading

-- 10. CourseEnrollments (Enrollments)
INSERT INTO "CourseEnrollments" ("StudentID", "CourseID", "EnrollmentDate", "Status") VALUES
(1, 1, CURRENT_TIMESTAMP - interval '10 days', 'Completed'), -- EnrollmentID 1 (Olena on SQL)
(2, 1, CURRENT_DATE, 'In Progress'),                       -- EnrollmentID 2 (Dmytro on SQL)
(2, 3, CURRENT_DATE, 'In Progress');                       -- EnrollmentID 3 (Dmytro on Figma)

-- 11. StudentProgress (Progress)
INSERT INTO "StudentProgress" ("EnrollmentID", "LessonID", "IsCompleted", "DateCompleted") VALUES
(1, 1, TRUE, CURRENT_TIMESTAMP - interval '9 days'), -- Olena completed lesson 1
(1, 2, TRUE, CURRENT_TIMESTAMP - interval '8 days'), -- Olena completed lesson 2
(2, 1, TRUE, CURRENT_TIMESTAMP - interval '1 day'); -- Dmytro completed lesson 1

-- 12. Assignments
INSERT INTO "Assignments" ("CourseID", "Title", "MaxPoints", "DueDate") VALUES
(1, 'Final DML Test', 100, CURRENT_TIMESTAMP + interval '7 days'), -- AssignmentID 1 (for Course 1)
(3, 'Business Card Prototype', 50, CURRENT_TIMESTAMP + interval '10 days');      -- AssignmentID 2 (for Course 3)

-- 13. Grades
INSERT INTO "Grades" ("AssignmentID", "StudentID", "PointsEarned", "Feedback", "LastModifiedByUserID", "LastModifiedDate") VALUES
(1, 1, 95, 'Excellent work!', 2, CURRENT_TIMESTAMP); -- GradeID 1 (Olena received 95 for the test)

-- 14. Comments
INSERT INTO "Comments" ("LessonID", "UserID", "CommentText", "DatePosted") VALUES
(3, 4, 'Are there additional materials on DML?', CURRENT_TIMESTAMP), -- Student 1
(3, 2, 'Check the attached resources.', CURRENT_TIMESTAMP);  -- Instructor 1

-- 15. AuditLog (Initial entry for system start)
INSERT INTO "AuditLog" ("UserID", "ActionType", "EntityName", "EntityID")
VALUES (2, 'CREATE', 'Courses', 1);