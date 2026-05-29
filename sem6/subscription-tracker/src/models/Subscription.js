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
  billingCycle: {
    type: String,
    enum: ['weekly', 'monthly', 'yearly', 'custom'],
    default: 'monthly',
  },
  customCycleDays: {
    type: Number,
    default: null,
  },
  nextBillingDate: {
    type: Date,
    required: true,
  },
  startDate: {
    type: Date,
    default: Date.now,
  },
  source: {
    type: String,
    enum: ['monobank', 'csv', 'manual'],
    default: 'manual',
  },
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
  reminderDays: {
    type: Number,
    default: null,
  },
}, { timestamps: true });

module.exports = mongoose.model('Subscription', subscriptionSchema);