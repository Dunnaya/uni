const { pool } = require('./db.config');
const UserRepository = require('./repositories/UserRepository');
const CourseRepository = require('./repositories/CourseRepository');

class UnitOfWork {
    constructor() {
        // the client for the current transaction if any
        this.client = null;
        this.userRepository = null;
        this.courseRepository = null;
    }

    async beginTransaction() {
        this.client = await pool.connect();
        await this.client.query('BEGIN');
        
        // initializing repositories with an active client
        this.userRepository = new UserRepository(this.client);
        this.courseRepository = new CourseRepository(this.client);
    }

    async commit() {
        if (!this.client) throw new Error("Transaction not started.");
        await this.client.query('COMMIT');
        this.client.release();
        this.client = null;
    }

    async rollback() {
        if (!this.client) throw new Error("Transaction not started.");
        await this.client.query('ROLLBACK');
        this.client.release();
        this.client = null;
    }

    // method for working without transactions (for simple SELECT)
    getRepository(name) {
        // if a transaction is NOT active, return the repository initialized with the Pool
        if (!this.client) {
            if (name === 'user') return new UserRepository(pool);
            if (name === 'course') return new CourseRepository(pool);
        }
        
        // if a transaction IS active, return the repository with the active Client
        if (name === 'user') return this.userRepository;
        if (name === 'course') return this.courseRepository;
        
        throw new Error(`Repository ${name} not found.`);
    }

    async endPool() {
        if (this.client) {
            // if error occurs during rollback/commit, client should still be released
            this.client.release();
        }
        await pool.end();
    }
}

module.exports = UnitOfWork;