const mongoose = require('mongoose');

const connectDB = async () => {
  try {
    await mongoose.connect(process.env.MONGODB_URI);
    console.log('MongoDB CONNECTED');
  } catch (err) {
    console.error('MongoDB ERROR:', err.message);
    process.exit(1);
  }
};

module.exports = connectDB;