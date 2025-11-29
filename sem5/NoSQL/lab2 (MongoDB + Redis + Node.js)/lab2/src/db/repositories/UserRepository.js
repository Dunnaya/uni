const BaseRepository = require('./BaseRepository');

class UserRepository extends BaseRepository {
    constructor(client) {
        super(client);
    }

    // 1. VIEW call (for getting active users)
    async getActiveUsers() {
        // in this example, there is no VIEW for users only, so we show SELECT with a filter but in real case, a VIEW should be created
        const res = await this.client.query('SELECT "UserID", "FirstName", "LastName", "Email" FROM "Users" WHERE "IsActive" = TRUE');
        return res.rows;
    }

    // 2. STORED PROCEDURE call (for Soft Delete)
    async softDeleteUser(userId) {
        // call the sp_SoftDeleteUser procedure, which handles IsActive and AuditLog
        const res = await this.client.query('CALL sp_SoftDeleteUser($1)', [userId]);
        return res.rowCount > 0;
    }
}

module.exports = UserRepository;