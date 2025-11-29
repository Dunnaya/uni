-- STORED PROCEDURES --

-- 1. Stored Procedure: Soft Delete User
CREATE OR REPLACE PROCEDURE sp_SoftDeleteUser(user_id_p INT)
LANGUAGE plpgsql
AS $$
BEGIN
    UPDATE "Users"
    SET "IsActive" = FALSE
    WHERE "UserID" = user_id_p;
    -- Adding an entry to the AuditLog (this can be done with a trigger, but for the SP example - here)
    INSERT INTO "AuditLog" ("UserID", "ActionType", "EntityName", "EntityID")
    VALUES (user_id_p, 'SOFT_DELETE', 'Users', user_id_p);
END;
$$;

-- 2. Stored Procedure: Update Assessment and Audit
CREATE OR REPLACE PROCEDURE sp_UpdateGradeAndAudit(
    grade_id_p INT,
    points_earned_p INT,
    feedback_p TEXT,
    modified_by_user_id_p INT -- User making changes (for LastModifiedByUserID)
)
LANGUAGE plpgsql
AS $$
BEGIN
    UPDATE "Grades"
    SET
        "PointsEarned" = points_earned_p,
        "Feedback" = feedback_p,
        "LastModifiedByUserID" = modified_by_user_id_p,
        "LastModifiedDate" = CURRENT_TIMESTAMP
    WHERE "GradeID" = grade_id_p;

    -- Record of change in AuditLog
    INSERT INTO "AuditLog" ("UserID", "ActionType", "EntityName", "EntityID")
    VALUES (modified_by_user_id_p, 'UPDATE', 'Grades', grade_id_p);
END;
$$;

-- 3. Procedure: Soft Delete Comments
-- Sets IsDeleted = TRUE for a comment.
CREATE OR REPLACE PROCEDURE sp_LogCommentDeletion(comment_id_p INT, user_id_p INT)
LANGUAGE plpgsql
AS $$
BEGIN
    UPDATE "Comments"
    SET "IsDeleted" = TRUE
    WHERE "CommentID" = comment_id_p;

    INSERT INTO "AuditLog" ("UserID", "ActionType", "EntityName", "EntityID")
    VALUES (user_id_p, 'SOFT_DELETE', 'Comments', comment_id_p);
END;
$$;


-- 4. Procedure: Enrollment
-- Creates an entry in CourseEnrollments.
CREATE OR REPLACE PROCEDURE sp_EnrollStudent(student_id_p INT, course_id_p INT)
LANGUAGE plpgsql
AS $$
DECLARE
    v_user_id INT;
BEGIN
    -- Inserting an enrollment record. A unique index will prevent duplication.
    INSERT INTO "CourseEnrollments" ("StudentID", "CourseID", "EnrollmentDate", "Status")
    VALUES (student_id_p, course_id_p, CURRENT_TIMESTAMP, 'In Progress');
    
    -- Audit
    SELECT "UserID" INTO v_user_id FROM "Students" WHERE "StudentID" = student_id_p;
    INSERT INTO "AuditLog" ("UserID", "ActionType", "EntityName", "EntityID")
    VALUES (v_user_id, 'ENROLL', 'Courses', course_id_p);
END;
$$;


-- 5. Procedure: Mark Lesson Completed
-- Updates or creates a record in StudentProgress.
CREATE OR REPLACE PROCEDURE sp_MarkLessonCompleted(enrollment_id_p INT, lesson_id_p INT)
LANGUAGE plpgsql
AS $$
BEGIN
    INSERT INTO "StudentProgress" ("EnrollmentID", "LessonID", "IsCompleted", "DateCompleted")
    VALUES (enrollment_id_p, lesson_id_p, TRUE, CURRENT_TIMESTAMP)
    -- If the record already exists, update it (ON CONFLICT DO UPDATE)
    ON CONFLICT ("EnrollmentID", "LessonID") DO UPDATE 
    SET "IsCompleted" = TRUE, "DateCompleted" = CURRENT_TIMESTAMP;
END;
$$;

-- USER FUNCTIONS --

-- Function: Calculate the total course duration
CREATE OR REPLACE FUNCTION fn_GetTotalCourseDuration(course_id_p INT)
RETURNS INT
LANGUAGE plpgsql
AS $$
DECLARE
    total_duration_v INT;
BEGIN
    SELECT COALESCE(SUM(l."Duration"), 0) INTO total_duration_v
    FROM "Lessons" l
    JOIN "Sections" s ON l."SectionID" = s."SectionID"
    WHERE s."CourseID" = course_id_p;

    RETURN total_duration_v;
END;
$$;

-- TRIGGER FUNCTIONS --

-- 1. Trigger function for updating modification fields Lessons
CREATE OR REPLACE FUNCTION fn_UpdateLessonModification()
RETURNS TRIGGER AS $$
BEGIN
    -- Automatically set the current modification date
    NEW."LastModifiedDate" = CURRENT_TIMESTAMP;
    RETURN NEW;
END;
$$ LANGUAGE plpgsql;

-- 1.2. Create a trigger on the Lessons table
CREATE OR REPLACE TRIGGER tr_UpdateLesson
BEFORE UPDATE ON "Lessons"
FOR EACH ROW
EXECUTE FUNCTION fn_UpdateLessonModification();

-- 2. Trigger function: Automatically update modification fields Grades
CREATE OR REPLACE FUNCTION fn_UpdateGradeModification()
RETURNS TRIGGER AS $$
BEGIN
    -- Set the current modification date
    NEW."LastModifiedDate" = CURRENT_TIMESTAMP;
    RETURN NEW;
END;
$$ LANGUAGE plpgsql;

-- 2.1. Create a trigger on the Grades table
CREATE OR REPLACE TRIGGER tr_UpdateGrade
BEFORE UPDATE ON "Grades"
FOR EACH ROW
EXECUTE FUNCTION fn_UpdateGradeModification();

-- VIEWS --

-- 1. View: List of active and published courses (for the catalog)
CREATE OR REPLACE VIEW v_ActiveCourseList AS
SELECT
    c."CourseID",
    c."Title",
    c."Description",
    c."Price",
    fn_GetTotalCourseDuration(c."CourseID") AS "TotalDuration", -- Using the function
    cat."CategoryName",
    u."FirstName" || ' ' || u."LastName" AS "InstructorName"
FROM "Courses" c
JOIN "Categories" cat ON c."CategoryID" = cat."CategoryID"
JOIN "Instructors" i ON c."InstructorID" = i."InstructorID"
JOIN "Users" u ON i."UserID" = u."UserID"
-- Фільтр Soft Delete
WHERE c."IsDeleted" = FALSE AND c."IsPublished" = TRUE AND u."IsActive" = TRUE;

-- 2. View: Student Performance Report
CREATE OR REPLACE VIEW v_StudentPerformance AS
SELECT
    u."FirstName" || ' ' || u."LastName" AS "StudentName",
    c."Title" AS "CourseTitle",
    AVG(g."PointsEarned")::NUMERIC(5,2) AS "AverageGrade" -- Calculate Grade Point Average
FROM "Grades" g
JOIN "Students" s ON g."StudentID" = s."StudentID"
JOIN "Users" u ON s."UserID" = u."UserID"
JOIN "Assignments" a ON g."AssignmentID" = a."AssignmentID"
JOIN "Courses" c ON a."CourseID" = c."CourseID"
GROUP BY 1, 2;