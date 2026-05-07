const mongoose = require('mongoose');

const transactionSchema = new mongoose.Schema({
  userId: {
    type: mongoose.Schema.Types.ObjectId,
    ref: 'User',
    required: true,
    index: true,
  },
  // unique transaction id from source (e.g. monobank transaction id or csv line hash)
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
  amount: { type: Number, required: true },   // negative for expenses
  currency: { type: String, default: 'UAH' },
  description: { type: String },
  mcc: { type: Number },
  date: { type: Date, required: true },
  isSubscription: { type: Boolean, default: false },
  // link to sub if identified as subcharge
  subscriptionId: {
    type: mongoose.Schema.Types.ObjectId,
    ref: 'Subscription',
    default: null,
  },
}, { timestamps: true });

module.exports = mongoose.model('Transaction', transactionSchema);