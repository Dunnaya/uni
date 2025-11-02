# Варіант 14: Система управління навчальними курсами.

1) Розробити схему бази даних з вказанням зв’язків між об’єктами, полями, ключами й тд.
(https://dbdiagram.io/home)

2) Мінімальна кількість сутностей 15.
- Roles, Users, Instructors, Students, Categories, Courses, Sections, Lessons, Resources, CourseEnrollments, StudentProgress, Assignments, Grades, AuditLog, Comments.

3) Сутності повинні спиратися на сформовані вимоги. Хоча б деякі сутності мають підтримувати “Soft delete”, деякі сутності мають зберігати дату останньої зміни даних та користувача що їх змінив.
- Soft Delete: поля IsActive (Users) та IsDeleted (Courses, Comments).
- Відстеження Змін: поля LastModifiedDate та LastModifiedByUserID в Lessons та Grades.
- Аудит: таблиця AuditLog для запису ключових дій.

4) Реалізувати модель у одній з реляційних СУБД.
- PostgreSQL

5) Створення й використання збережених процедур, користувацьких функцій, тригерів, розрізів даних (View), тощо. В першу чергу для роботи з сутностями з п.3 (мінімум 10).
- 5 Stored Procedures (SPs): sp_SoftDeleteUser, sp_UpdateGradeAndAudit, sp_LogCommentDeletion, sp_EnrollStudent, sp_MarkLessonCompleted
- 1 Function: fn_GetTotalCourseDuration
- 2 Triggers: tr_UpdateLesson, tr_UpdateGrade
- 2 Views: v_ActiveCourseList, v_StudentPerformance

6) Створення індексів (хоча б 2 різні типи)
- B-Tree: стандартні індекси, включаючи складений індекс idx_courses_catalog для прискорення фільтрації.
- GIN: idx_courses_search для забезпечення швидкого повнотекстового пошуку по курсах.

7) Робота з декількома сутностями БД з коду. Реалізувати комбінацію патернів Repository + Unit of Work для декількох логічно-зв'язаних сутностей, де усі запити будуть йти через використання збережених процедур та розрізів даних(для отримання даних).
- Node.js
- у db.config.js змінні передаються через .env файл.
- Repository Pattern: створено класи UserRepository та CourseRepository.
- Unit of Work (UoW) Pattern: клас UnitOfWork реалізований для керування транзакціями (атомарні операції або повністю фіксуються (COMMIT), або повністю скасовуються (ROLLBACK) у разі помилки (наприклад, порушення foreign key).).