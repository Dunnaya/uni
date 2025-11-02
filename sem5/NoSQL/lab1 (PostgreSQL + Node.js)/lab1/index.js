const UnitOfWork = require('./src/db/UnitOfWork');

async function runDemo() {
    const uow = new UnitOfWork();

    try {
        console.log(" 1. Reading Data via Repository (VIEW)");
        // get repository without starting a transaction (for simple SELECT)
        const courseRepoRead = uow.getRepository('course');
        
        // calls the v_ActiveCourseList VIEW
        const publishedCourses = await courseRepoRead.getPublishedCourses();
        console.log(`Found published courses: ${publishedCourses.length}`);
        console.log("Catalogue:", publishedCourses.map(c => c.Title));

        // -----------------------------------------------------------------
        
        console.log("\n 2. Data Modification via Repository (SP: Soft Delete)");
        const userRepo = uow.getRepository('user');
        
        // this calls sp_SoftDeleteUser(5) - Dmytro Shevchenko
        const userIdToSoftDelete = 5; 
        console.log(`Calling sp_SoftDeleteUser for UserID ${userIdToSoftDelete}...`);
        await userRepo.softDeleteUser(userIdToSoftDelete);
        console.log(` Success: UserID ${userIdToSoftDelete} successfully soft-deleted (IsActive = FALSE). Check AuditLog.`);
        
        const activeUsers = await userRepo.getActiveUsers();
        console.log(`Active users count after soft delete: ${activeUsers.length}`);


        // -----------------------------------------------------------------
        
        console.log("\n 3. Unit of Work (Transaction and Rollback)");
        // attempt to enroll a student (ID 1) in a non-existent course (ID 999).
        // the foreign key constraint will fail, triggering a ROLLBACK.
        
        const studentId = 1; 
        const invalidCourseId = 999;
        
        await uow.beginTransaction();
        console.log("Transaction started.");

        try {
            console.log(`Attempting enrollment for Student ID ${studentId} on invalid Course ID ${invalidCourseId}...`);
            
            // this will fail due to the CourseID FK constraint
            await uow.courseRepository.enrollStudent(studentId, invalidCourseId);
            
            await uow.commit(); // should not be reached
            console.log("Transaction committed."); 
        } catch (error) {
            console.error(" Transaction Error (ROLLBACK required):", error.message.split('\n')[0]);
            await uow.rollback(); // cancel all changes (if any occurred)
            console.log("Transaction successfully rolled back. No permanent changes to the database.");
        }

    } catch (error) {
        console.error("Critical Execution Error:", error);
    } finally {
        await uow.endPool();
        console.log("\n Demonstration finished.");
    }
}

runDemo();