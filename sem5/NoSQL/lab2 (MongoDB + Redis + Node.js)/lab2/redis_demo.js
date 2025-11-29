const UnitOfWork = require('./src/db/UnitOfWork');

// sleep func for demo purposes
const sleep = (ms) => new Promise(resolve => setTimeout(resolve, ms));

async function runRedisDemo() {
    console.log("\tTesting Redis");
    const uow = new UnitOfWork();
    // getting repository (it will connect to Redis inside)
    const courseRepo = uow.getRepository('course');

    try {
        // request #1
        console.log(" First request (cache should be empty)");
        console.time('Execution Time');
        const data1 = await courseRepo.getPublishedCourses();
        console.timeEnd('Execution Time');
        console.log(`Courses retrieved: ${data1.length}`);
        console.log("\n... waiting 2 seconds (simulating user activity) ...");
        await sleep(2000);

        // request #2
        console.log("\n Second request (should be retrieved from cache)");
        console.time('Execution Time');
        const data2 = await courseRepo.getPublishedCourses();
        console.timeEnd('Execution Time');
        console.log(`Courses retrieved: ${data2.length}`);
    } catch (error) {
        console.error("Error:", error);
    } finally {
        // closing connection
        // (in a real project this is done more properly, but for demo this will do)
        process.exit(0);
    }
}

runRedisDemo();