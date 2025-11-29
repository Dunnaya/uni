const { Client } = require('pg');
const fs = require('fs');

const client = new Client({
    user: 'user', host: 'localhost', database: 'lms_db', password: 'password', port: 5432
});

async function init() {
    await client.connect();
    // readiing schema file and executing
    const sql = fs.readFileSync('./src/sql/01_create_schema.sql').toString();
    await client.query(sql);
    console.log("The tables have been created.");
    await client.end();
}

init();