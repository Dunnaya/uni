const { Client } = require('pg');
const fs = require('fs');

const client = new Client({
    user: 'user', 
    host: 'localhost', 
    database: 'lms_db', 
    password: 'password', 
    port: 5432
});

async function setup() {
    try {
        await client.connect();
        console.log("Connecting to the database...");

        // loading logic (Views, Procedures)
        console.log("Creating Views and procedures...");
        const logicSql = fs.readFileSync('./src/sql/02_create_logic.sql').toString();
        await client.query(logicSql);
        console.log("Logic (Views) created.");

        // loading test data
        console.log("Loading test data...");
        try {
            const dataSql = fs.readFileSync('./src/sql/03_test_data.sql').toString();
            await client.query(dataSql);
            console.log("Test data added.");
        } catch (e) {
            console.log("! Data already exists or insertion error occurred (this is not critical):", e.message.split('\n')[0]);
        }

    } catch (err) {
        console.error("Error:", err.message);
    } finally {
        await client.end();
    }
}

setup();