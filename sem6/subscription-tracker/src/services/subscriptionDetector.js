const Transaction = require('../models/Transaction');
const Subscription = require('../models/Subscription');
const { calcNextBillingDate } = require('../utils/dateUtils');

// MCC codes, typical for subs
const SUBSCRIPTION_MCC_CODES = [
  5815, // digital (Apple, Google)
  5816, // games
  7372, // software (Microsoft, Adobe)
  4816, // comp networks / cloud services
  7993, // Netflix-like (video, streaming)
];

// key words in description
const SUBSCRIPTION_KEYWORDS = [
  'netflix', 'spotify', 'apple', 'google', 'microsoft', 'adobe',
  'icloud', 'youtube premium', 'amazon', 'dropbox', 'chatgpt',
  'claude', 'notion', 'figma', 'slack', 'zoom', 'subscription',
];

exports.detect = async (userId, rawTransactions) => {
  // save transactions in db
  const monobankService = require('./monobankService');
  await monobankService.saveTransactions(userId, rawTransactions);

  // all c transactions
  const allTx = await Transaction.find({ userId }).sort({ date: -1 });

  let newCount = 0;
  let updatedCount = 0;

  // group by MCC + similar amount + description
  const groups = groupPotentialSubscriptions(allTx);

  for (const group of groups) {
    if (group.transactions.length < 2) continue;  // min 2 orders needed

    const interval = detectInterval(group.transactions.map(t => t.date));
    if (!interval) continue;

    // this sub exists?
    const existing = await Subscription.findOne({
      userId,
      name: group.name,
      amount: group.amount,
    });

    const lastDate = group.transactions[0].date;
    const nextBillingDate = calcNextBillingDate(lastDate, interval.cycle, interval.days);

    if (!existing) {
      await Subscription.create({
        userId,
        name: group.name,
        amount: Math.abs(group.amount),
        currency: 'UAH',
        billingCycle: interval.cycle,
        customCycleDays: interval.days,
        nextBillingDate,
        startDate: group.transactions[group.transactions.length - 1].date,
        source: 'monobank',
        mcc: group.mcc,
        isActive: true,
      });
      newCount++;
    } else {
      // refresh next billing date
      existing.nextBillingDate = nextBillingDate;
      await existing.save();
      updatedCount++;
    }
  }

  return { new: newCount, updated: updatedCount };
};

function groupPotentialSubscriptions(transactions) {
  const groups = {};

  for (const tx of transactions) {
    if (tx.amount >= 0) continue;  // expenses only

    const isSubByMcc = SUBSCRIPTION_MCC_CODES.includes(tx.mcc);
    const desc = (tx.description || '').toLowerCase();
    const isSubByKeyword = SUBSCRIPTION_KEYWORDS.some(kw => desc.includes(kw));

    if (!isSubByMcc && !isSubByKeyword) continue;

    // group key: MCC + rounded amount (±5% tolerance)
    const roundedAmount = Math.round(tx.amount / 100) * 100;
    const key = `${tx.mcc}_${roundedAmount}_${extractName(tx.description)}`;

    if (!groups[key]) {
      groups[key] = {
        name: extractName(tx.description),
        amount: tx.amount,
        mcc: tx.mcc,
        transactions: [],
      };
    }
    groups[key].transactions.push(tx);
  }

  return Object.values(groups);
}

function extractName(description = '') {
  // attempt to find service name
  const lower = description.toLowerCase();
  for (const kw of SUBSCRIPTION_KEYWORDS) {
    if (lower.includes(kw)) return capitalize(kw);
  }
  // first word as fallback
  return description.split(/[\s*]/)[0] || 'Unknown subscription';
}

function detectInterval(dates) {
  if (dates.length < 2) return null;

  const sorted = [...dates].sort((a, b) => a - b);
  const diffs = [];
  for (let i = 1; i < sorted.length; i++) {
    diffs.push((sorted[i] - sorted[i - 1]) / (1000 * 60 * 60 * 24)); // in days
  }
  const avgDays = diffs.reduce((a, b) => a + b, 0) / diffs.length;

  if (avgDays < 9)   return { cycle: 'weekly', days: 7 };
  if (avgDays < 35)  return { cycle: 'monthly', days: 30 };
  if (avgDays < 100) return { cycle: 'monthly', days: 30 }; // quarterly — monthly
  if (avgDays < 400) return { cycle: 'yearly', days: 365 };

  return { cycle: 'custom', days: Math.round(avgDays) };
}

function capitalize(str) {
  return str.charAt(0).toUpperCase() + str.slice(1);
}