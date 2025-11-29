class BaseRepository {
    constructor(clientOrPool) {
        this.client = clientOrPool;
    }
    
    // use this.client.query, no matter Pool or Client
    async query(text, params) {
        return this.client.query(text, params);
    }
}

module.exports = BaseRepository;