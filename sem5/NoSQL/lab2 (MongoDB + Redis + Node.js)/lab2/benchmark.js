const { Pool } = require('pg');
const mongoose = require('mongoose');
const MongoCourse = require('./src/mongo/CourseModel');

// setting up connections
const pgPool = new Pool({
    user: 'user', host: 'localhost', database: 'lms_db', password: 'password', port: 5432
});
const mongoUri = 'mongodb://localhost:27017/lms_no_sql';

async function runBenchmark() {
    // connection
    await mongoose.connect(mongoUri);
    const pgClient = await pgPool.connect();
    console.log("\tStarting Benchmark");

    try {
        // stage 1: data generation in SQL
        // creating 1 course, 100 sections, 20 lessons each (2000 total)
        // this simulates "heavy" data where SQL starts to lag on JOINs
        
        await pgClient.query('BEGIN');
        
        // creating instructor and category (if not exists, to avoid FK error)
        await pgClient.query(`INSERT INTO "Roles" ("RoleName") VALUES ('BenchmarkRole') ON CONFLICT DO NOTHING`);
        await pgClient.query(`INSERT INTO "Users" ("RoleID", "Email", "PasswordHash") VALUES (1, 'bench@test.com', 'hash') ON CONFLICT DO NOTHING`);
        await pgClient.query(`INSERT INTO "Instructors" ("UserID") VALUES ((SELECT "UserID" FROM "Users" WHERE "Email"='bench@test.com')) ON CONFLICT DO NOTHING`);
        await pgClient.query(`INSERT INTO "Categories" ("CategoryName") VALUES ('BenchmarkCat') ON CONFLICT DO NOTHING`);

        const instructorId = (await pgClient.query(`SELECT "InstructorID" FROM "Instructors" LIMIT 1`)).rows[0].InstructorID;
        const categoryId = (await pgClient.query(`SELECT "CategoryID" FROM "Categories" LIMIT 1`)).rows[0].CategoryID;

        // creating course
        const cRes = await pgClient.query(`INSERT INTO "Courses" ("InstructorID", "CategoryID", "Title", "IsPublished") VALUES ($1, $2, 'Heavy SQL Course', TRUE) RETURNING "CourseID"`, [instructorId, categoryId]);
        const courseId = cRes.rows[0].CourseID;

        // filling with sections and lessons
        console.log(`Generating SQL data for Course ID: ${courseId} (2000 lessons)...`);
        for (let i = 0; i < 100; i++) {
            const sRes = await pgClient.query(`INSERT INTO "Sections" ("CourseID", "Title", "OrderNumber") VALUES ($1, $2, $3) RETURNING "SectionID"`, [courseId, `Sec ${i}`, i]);
            const sectionId = sRes.rows[0].SectionID;
            
            for (let j = 0; j < 20; j++) {
                await pgClient.query(`INSERT INTO "Lessons" ("SectionID", "Title", "Duration") VALUES ($1, $2, 15)`, [sectionId, `Lesson ${j}`]);
            }
        }
        await pgClient.query('COMMIT');
        console.log("SQL data generated.");


        // stage 2: migration to MongoDB
        // we read data from SQL and write it in a convenient format to Mongo
        
        console.log("Migrating data to MongoDB...");
        const sectionsRes = await pgClient.query(`SELECT * FROM "Sections" WHERE "CourseID" = $1`, [courseId]);
        let mongoSections = [];
        
        for (let sec of sectionsRes.rows) {
            const lessonsRes = await pgClient.query(`SELECT * FROM "Lessons" WHERE "SectionID" = $1`, [sec.SectionID]);
            mongoSections.push({
                sql_section_id: sec.SectionID,
                title: sec.Title,
                order: sec.OrderNumber,
                lessons: lessonsRes.rows.map(l => ({ title: l.Title, duration: l.Duration }))
            });
        }

        await MongoCourse.create({
            sql_course_id: courseId,
            title: 'Heavy Mongo Course',
            sections: mongoSections
        });
        console.log("Migration complete.");


        // stage 3: speed comparison (read test)
        console.log("\n>>> Starting race: get full course syllabus");
        console.log("Comparing getting full course syllabus JSON tree (structure for frontend)");

        // warm up
        await pgClient.query(`SELECT 1`);
        await mongoose.connection.db.collection('mongocourses').findOne({ sql_course_id: courseId });

        const ITERATIONS = 1000;
        console.log(`Running ${ITERATIONS} iterations...`);

        // test SQL (fair test: making SQL build JSON like Mongo does)
        console.time(`SQL (Complex JSON Build x${ITERATIONS})`);
        for (let i = 0; i < ITERATIONS; i++) {
            await pgClient.query(`
                SELECT json_build_object(
                    'title', c."Title",
                    'sections', (
                        SELECT json_agg(json_build_object(
                            'title', s."Title",
                            'lessons', (
                                SELECT json_agg(json_build_object('title', l."Title", 'duration', l."Duration"))
                                FROM "Lessons" l WHERE l."SectionID" = s."SectionID"
                            )
                        ))
                        FROM "Sections" s WHERE s."CourseID" = c."CourseID"
                    )
                )
                FROM "Courses" c 
                WHERE c."CourseID" = $1
            `, [courseId]);
        }
        console.timeEnd(`SQL (Complex JSON Build x${ITERATIONS})`);

        // test NoSQL (native driver speed)
        // using native driver to remove Mongoose library overhead
        console.time(`NoSQL (Native Find x${ITERATIONS})`);
        const collection = mongoose.connection.db.collection('mongocourses');
        for (let i = 0; i < ITERATIONS; i++) {
            await collection.findOne({ sql_course_id: courseId });
        }
        console.timeEnd(`NoSQL (Native Find x${ITERATIONS})`);

    } catch (e) {
        console.error(e);
        await pgClient.query('ROLLBACK');
    } finally {
        await pgClient.end();
        await mongoose.disconnect();
    }
}

runBenchmark();