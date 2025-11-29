const BaseRepository = require('./BaseRepository');
const { createClient } = require('redis');

class CourseRepository extends BaseRepository {
    constructor(client) {
        super(client);
        // creating Redis client
        this.redisClient = createClient({ url: 'redis://localhost:6379' });
        this.redisClient.on('error', (err) => console.log('Redis Client Error', err));
        // connecting (this is async but i think for lab it's okay to run like this)
        this.redisClient.connect();
    }

    async getPublishedCourses() {
        const CACHE_KEY = 'courses:published_list';

        try {
            // try to find data in Redis
            const cachedData = await this.redisClient.get(CACHE_KEY);
            
            if (cachedData) {
                console.log('\x1b[32m%s\x1b[0m', '⚡ [REDIS HIT] Data retrieved instantly from cache!');
                return JSON.parse(cachedData); // return data from Redis without touching SQL
            }
        } catch (e) {
            console.error('Redis error:', e);
        }

        // if cache is empty — go to PostgreSQL (as before)
        console.log('\x1b[33m%s\x1b[0m', '🐢 [POSTGRES MISS] Cache is empty, querying the DB...');
        
        // query the View
        const res = await this.client.query('SELECT * FROM v_ActiveCourseList');
        const rows = res.rows;

        try {
            // save the result in Redis for 60 seconds (to make the next request fast)
            await this.redisClient.set(CACHE_KEY, JSON.stringify(rows), { EX: 60 });
            console.log('Data saved in Redis cache for 60 seconds.');
        } catch (e) {
            console.error('Redis write error:', e);
        }

        return rows;
    }
    
    async enrollStudent(studentId, courseId) {
        const res = await this.client.query('CALL sp_EnrollStudent($1, $2)', [studentId, courseId]);
        return res.rowCount > 0;
    }
}

module.exports = CourseRepository;