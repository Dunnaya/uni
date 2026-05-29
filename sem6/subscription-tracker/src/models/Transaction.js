const mongoose = require('mongoose');

const transactionSchema = new mongoose.Schema({
  userId: {
    type: mongoose.Schema.Types.ObjectId,
    ref: 'User',
    required: true,
    index: true,
  },
  externalId: {
    type: String,
    unique: true,
    sparse: true,
  },
  source: {
    type: String,
    enum: ['monobank', 'csv'],
    required: true,
  },
  amount: { type: Number, required: true },
  currency: { type: String, default: 'UAH' },
  description: { type: String, default: '' },
  counterName: { type: String, default: '' },
  mcc: { type: Number },
  date: { type: Date, required: true },
  isSubscription: { type: Boolean, default: false },
  subscriptionId: {
    type: mongoose.Schema.Types.ObjectId,
    ref: 'Subscription',
    default: null,
  },
}, { timestamps: true });

module.exports = mongoose.model('Transaction', transactionSchema);