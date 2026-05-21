const Transaction = require('../models/Transaction');
const Subscription = require('../models/Subscription');
const { calcNextBillingDate } = require('../utils/dateUtils');

const SUBSCRIPTION_MCC = [5815, 5816, 7372, 4816, 4899];

// service names that are unambiguously subscriptions
const KEYWORDS = [
  'netflix', 'spotify', 'apple', 'google', 'microsoft', 'adobe',
  'icloud', 'youtube', 'amazon', 'dropbox', 'chatgpt',
  'claude', 'notion', 'figma', 'slack', 'zoom', 'subscription',
];

exports.detect = async (userId, rawTransactions) => {
  const monobankService = require('./monobankService');
  await monobankService.saveTransactions(userId, rawTransactions);

  const allTx = await Transaction.find({ userId }).sort({ date: -1 });
  const groups = groupByService(allTx);

  let newCount = 0;
  let updatedCount = 0;

  for (const group of groups) {
    const knownByName = KEYWORDS.some(kw => group.name.toLowerCase().includes(kw));

    if (group.transactions.length < 2) {
      // for a single transaction we only create a subscription if the service
      // name is in the known keywords list - specific enough to avoid false positives
      // (e.g. "YouTube" is fine, a random App Store game purchase is not)
      if (!knownByName) continue;
    }

    const interval = group.transactions.length >= 2
      ? detectInterval(group.transactions.map(t => t.date))
      : { cycle: 'monthly', days: null };

    if (!interval) continue;

    const amountUah = Math.abs(group.amount) / 100;
    const existing = await Subscription.findOne({
      userId,
      name: group.name,
      amount: { $gte: amountUah * 0.95, $lte: amountUah * 1.05 },
    });

    const lastDate = group.transactions[0].date;
    const nextBillingDate = calcNextBillingDate(lastDate, interval.cycle, interval.days);

    if (!existing) {
      await Subscription.create({
        userId,
        name: group.name,
        amount: amountUah,
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
      existing.nextBillingDate = nextBillingDate;
      await existing.save();
      updatedCount++;
    }
  }

  return { new: newCount, updated: updatedCount };
};

function groupByService(transactions) {
  const groups = {};

  for (const tx of transactions) {
    if (tx.amount >= 0) continue;

    const byMcc = SUBSCRIPTION_MCC.includes(tx.mcc);
    const desc = (tx.description || '').toLowerCase();
    const byKeyword = KEYWORDS.some(kw => desc.includes(kw));

    if (!byMcc && !byKeyword) continue;

    const roundedAmount = Math.round(tx.amount / 100) * 100;
    const name = extractName(tx.description);
    const key = `${tx.mcc}_${roundedAmount}_${name}`;

    if (!groups[key]) {
      groups[key] = { name, amount: tx.amount, mcc: tx.mcc, transactions: [] };
    }
    groups[key].transactions.push(tx);
  }

  return Object.values(groups);
}

function extractName(description = '') {
  const lower = description.toLowerCase();
  for (const kw of KEYWORDS) {
    if (lower.includes(kw)) {
      return kw.charAt(0).toUpperCase() + kw.slice(1);
    }
  }
  return description.split(/[\s*]/)[0] || 'Unknown';
}

function detectInterval(dates) {
  if (dates.length < 2) return null;

  const sorted = [...dates].sort((a, b) => a - b);
  const diffs = [];
  for (let i = 1; i < sorted.length; i++) {
    diffs.push((sorted[i] - sorted[i - 1]) / (1000 * 60 * 60 * 24));
  }
  const avg = diffs.reduce((a, b) => a + b, 0) / diffs.length;

  if (avg < 9)   return { cycle: 'weekly',  days: null };
  if (avg < 100) return { cycle: 'monthly', days: null };
  if (avg < 400) return { cycle: 'yearly',  days: null };
  return { cycle: 'custom', days: Math.round(avg) };
}