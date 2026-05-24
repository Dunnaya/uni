const Transaction = require('../models/Transaction');
const Subscription = require('../models/Subscription');
const { calcNextBillingDate } = require('../utils/dateUtils');

// MCC 5816 (games) excluded — Steam purchases are one-time, not subscriptions
const SUBSCRIPTION_MCC = [5815, 5817, 7372, 4816, 4899, 7995, 5968];

const PAYMENT_INTERMEDIARIES = new Set([
  'xsolla', 'liqpay', 'fondy', 'wayforpay', 'portmone', 'easypay', 'ipay', 'paypal', 'stripe',
]);

const KNOWN_SERVICES = new Set([
  'netflix', 'spotify', 'apple', 'google', 'microsoft', 'adobe',
  'icloud', 'youtube', 'amazon', 'dropbox', 'chatgpt', 'openai',
  'claude', 'notion', 'figma', 'slack', 'zoom', 'patreon',
  'twitch', 'duolingo', 'canva', 'xbox', 'playstation',
  'hulu', 'disney', 'hbo', 'deezer', 'tidal', 'lastpass',
  'nordvpn', 'expressvpn', 'grammarly', 'evernote', 'todoist',
]);

const GENERIC_SIGNALS = new Set([
  'subscription', 'premium', 'monthly', 'annual', 'membership', 'підписка', 'преміум',
]);

// longer names first so substring match picks the most specific one
const KNOWN_ORDERED = [
  'microsoft', 'playstation', 'nordvpn', 'expressvpn', 'grammarly',
  'lastpass', 'duolingo', 'patreon', 'netflix', 'spotify',
  'youtube', 'dropbox', 'chatgpt', 'openai', 'disney', 'twitch',
  'icloud', 'amazon', 'claude', 'notion', 'canva', 'apple',
  'google', 'adobe', 'figma', 'slack', 'hulu', 'zoom',
  'xbox', 'hbo', 'deezer', 'tidal', 'todoist', 'evernote',
];

exports.detect = async (userId, rawTransactions) => {
  const monobankService = require('./monobankService');
  await monobankService.saveTransactions(userId, rawTransactions);

  const allTx = await Transaction.find({ userId }).sort({ date: -1 });
  const groups = groupByService(allTx);

  let newCount = 0;
  let updatedCount = 0;

  for (const group of groups) {
    const isKnown = classifyGroup(group);
    if (!isKnown && group.transactions.length < 2) continue;

    const interval = group.transactions.length >= 2
      ? detectInterval(group.transactions.map(t => t.date))
      : null;
    const { cycle, days } = interval ?? { cycle: 'monthly', days: null };

    const amountUah = Math.abs(group.amount) / 100;
    const existing = await Subscription.findOne({
      userId,
      name: group.name,
      amount: { $gte: amountUah * 0.9, $lte: amountUah * 1.1 },
    });

    const nextBillingDate = calcNextBillingDate(group.transactions[0].date, cycle, days);

    if (!existing) {
      await Subscription.create({
        userId,
        name:            group.name,
        amount:          amountUah,
        currency:        'UAH',
        billingCycle:    cycle,
        customCycleDays: days,
        nextBillingDate,
        startDate:       group.transactions[group.transactions.length - 1].date,
        source:          'monobank',
        mcc:             group.mcc,
        isActive:        true,
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

function classifyGroup(group) {
  const text = group.transactions
    .map(tx => `${tx.counterName || ''} ${tx.description || ''}`)
    .join(' ')
    .toLowerCase();
  return [...KNOWN_SERVICES].some(kw => text.includes(kw)) || KNOWN_SERVICES.has(group.name.toLowerCase());
}

function groupByService(transactions) {
  const groups = {};

  for (const tx of transactions) {
    if (tx.amount >= 0) continue;

    const combined = `${tx.counterName || ''} ${tx.description || ''}`.toLowerCase();
    const byMcc     = SUBSCRIPTION_MCC.includes(tx.mcc);
    const byKnown   = [...KNOWN_SERVICES].some(kw => combined.includes(kw));
    const bySignal  = [...GENERIC_SIGNALS].some(kw => combined.includes(kw));

    if (!byMcc && !byKnown && !bySignal) continue;

    const name = extractName(tx.counterName, tx.description);
    if (PAYMENT_INTERMEDIARIES.has(name.toLowerCase())) continue;

    const key = byKnown
      ? `known_${name.toLowerCase()}`
      : `${tx.mcc}_${Math.round(tx.amount / 100) * 100}_${name}`;

    if (!groups[key]) {
      groups[key] = { name, amount: tx.amount, mcc: tx.mcc, transactions: [] };
    } else if (Math.abs(tx.amount) > Math.abs(groups[key].amount)) {
      groups[key].amount = tx.amount;
    }
    groups[key].transactions.push(tx);
  }

  return Object.values(groups);
}

function extractName(counterName = '', description = '') {
  for (const src of [counterName, description]) {
    const words = src.toLowerCase().split(/[\s.,*/\-()]+/).filter(Boolean);
    const match = words.find(w => KNOWN_SERVICES.has(w));
    if (match) return match.charAt(0).toUpperCase() + match.slice(1);
  }
  for (const src of [counterName, description]) {
    const lower = src.toLowerCase();
    for (const kw of KNOWN_ORDERED) {
      if (lower.includes(kw)) return kw.charAt(0).toUpperCase() + kw.slice(1);
    }
  }
  const first = counterName.trim().split(/[\s,*]+/)[0];
  if (first && first.length > 1) return first;
  return description.split(/[\s*]/)[0] || 'Unknown';
}

function detectInterval(dates) {
  const sorted = [...dates].sort((a, b) => a - b);
  const diffs = [];
  for (let i = 1; i < sorted.length; i++) {
    diffs.push((sorted[i] - sorted[i - 1]) / (1000 * 60 * 60 * 24));
  }

  // Guard: fewer than 2 dates means no diffs — default to monthly
  if (!diffs.length) return { cycle: 'monthly', days: null };

  const avg = diffs.reduce((a, b) => a + b, 0) / diffs.length;

  // Guard: same-day duplicates → avg=0 → treat as monthly
  if (avg < 1)   return { cycle: 'monthly', days: null };
  if (avg < 9)   return { cycle: 'weekly',  days: null };
  if (avg < 100) return { cycle: 'monthly', days: null };
  if (avg < 400) return { cycle: 'yearly',  days: null };
  return { cycle: 'custom', days: Math.round(avg) };
}