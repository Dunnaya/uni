const BaseRepository = require('./BaseRepository');

class CourseRepository extends BaseRepository {
    constructor(client) {
        super(client);
    }

    // 1. VIEW call (for getting published courses)
    async getPublishedCourses() {
        // using v_ActiveCourseList to get only published courses
        const res = await this.client.query('SELECT * FROM v_ActiveCourseList');
        return res.rows;
    }
    
    // 2. STORED PROCEDURE call (for transactional enrollment)
    async enrollStudent(studentId, courseId) {
        // using sp_EnrollStudent, which registers the student and adds audit log
        const res = await this.client.query('CALL sp_EnrollStudent($1, $2)', [studentId, courseId]);
        return res.rowCount > 0;
    }
}

module.exports = CourseRepository;