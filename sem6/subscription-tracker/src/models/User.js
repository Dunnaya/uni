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
  // syphered token
  monobankToken: {
    type: String,          // AES-256 sypheresd string
    default: null,
  },
  monobankLastSync: {
    type: Date,
    default: null,
  },
  // tg
  telegramChatId: {
    type: String,
    default: null,
  },
  telegramLinked: {
    type: Boolean,
    default: false,
  },
  // reminreds settings
  defaultReminderDays: {
    type: Number,
    default: 3,
  },
}, { timestamps: true });

// hash passw before saving
userSchema.pre('save', async function (next) {
  if (!this.isModified('passwordHash')) return next();
  this.passwordHash = await bcrypt.hash(this.passwordHash, 12);
  next();
});

userSchema.methods.comparePassword = function (password) {
  return bcrypt.compare(password, this.passwordHash);
};

module.exports = mongoose.model('User', userSchema);