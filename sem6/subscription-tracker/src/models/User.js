const mongoose = require('mongoose');
const bcrypt = require('bcryptjs');

const userSchema = new mongoose.Schema({
  email: {
    type: String,
    required: true,
    unique: true,
    lowercase: true,
    trim: true,
  },
  passwordHash: {
    type: String,
    required: true,
  },
  // Increment this to invalidate all existing JWT tokens for this user
  // (e.g. after logout-all or password change).
  tokenVersion: {
    type: Number,
    default: 0,
  },
  monobankToken: {
    type: String,
    default: null,
  },
  monobankLastSync: {
    type: Date,
    default: null,
  },
  telegramChatId: {
    type: String,
    default: null,
  },
  telegramLinked: {
    type: Boolean,
    default: false,
  },
  telegramLinkToken: {
    type: String,
    default: null,
  },
  telegramLinkTokenExpiry: {
    type: Date,
    default: null,
  },
  defaultReminderDays: {
    type: Number,
    default: 3,
  },
}, { timestamps: true });

userSchema.pre('save', async function (next) {
  if (!this.isModified('passwordHash')) return next();
  this.passwordHash = await bcrypt.hash(this.passwordHash, 12);
  next();
});

userSchema.methods.comparePassword = function (password) {
  return bcrypt.compare(password, this.passwordHash);
};

module.exports = mongoose.model('User', userSchema);