const mongoose = require('mongoose');
const connectMongo = async () => {
    await mongoose.connect('mongodb://localhost:27017/lms_no_sql');
    console.log('MongoDB Connected');
};
module.exports = connectMongo;