const mongoose = require('mongoose');

const subscriptionSchema = new mongoose.Schema({
  userId: {
    type: mongoose.Schema.Types.ObjectId,
    ref: 'User',
    required: true,
    index: true,
  },
  name: {
    type: String,
    required: true,
    trim: true,
  },
  // filter categories
  category: {
    type: String,
    enum: ['entertainment', 'work', 'cloud', 'music', 'gaming', 'other'],
    default: 'other',
  },
  amount: {
    type: Number,
    required: true,
  },
  currency: {
    type: String,
    default: 'UAH',
  },
  // cycle: monthly / yearly / weekly / custom
  billingCycle: {
    type: String,
    enum: ['weekly', 'monthly', 'yearly', 'custom'],
    default: 'monthly',
  },
  customCycleDays: {
    type: Number,
    default: null,
  },
  // prognosed or entered
  nextBillingDate: {
    type: Date,
    required: true,
  },
  // 1st fixed charge date
  startDate: {
    type: Date,
    default: Date.now,
  },
  source: {
    type: String,
    enum: ['monobank', 'csv', 'manual'],
    default: 'manual',
  },
  // MCC-code or transaction description for manual entries
  mcc: {
    type: Number,
    default: null,
  },
  description: {
    type: String,
    default: '',
  },
  isActive: {
    type: Boolean,
    default: true,
  },
  isTrial: {
    type: Boolean,
    default: false,
  },
  trialEndDate: {
    type: Date,
    default: null,
  },
  // reminders settings
  reminderDays: {
    type: Number,
    default: null,   // null = use defaultReminderDays from User
  },
}, { timestamps: true });

module.exports = mongoose.model('Subscription', subscriptionSchema);