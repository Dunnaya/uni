const Transaction = require('../models/Transaction');
const Subscription = require('../models/Subscription');
const { calcNextBillingDate } = require('../utils/dateUtils');

// MCC codes that represent digital subscriptions / recurring services.
// 5815 = digital goods, 5817 = apps/subscriptions,
// 7372 = software, 4816 = info svcs, 4899 = cable/streaming,
// NOTE: 5816 (games) excluded — Steam/game purchases are one-time, not subscriptions.
const SUBSCRIPTION_MCC = [5815, 5817, 7372, 4816, 4899, 7995, 5968];

// Payment intermediaries — never create a subscription with these names.
const PAYMENT_INTERMEDIARIES = new Set([
  'xsolla', 'liqpay', 'fondy', 'wayforpay', 'portmone', 'easypay',
  'ipay', 'paypal', 'stripe',
]);

// LEVEL 1 — well-known services: always detect even with a single transaction.
const KNOWN_SERVICES = new Set([
  'netflix', 'spotify', 'apple', 'google', 'microsoft', 'adobe',
  'icloud', 'youtube', 'amazon', 'dropbox', 'chatgpt', 'openai',
  'claude', 'notion', 'figma', 'slack', 'zoom', 'patreon',
  'twitch', 'duolingo', 'canva', 'xbox', 'playstation',
  'hulu', 'disney', 'hbo', 'deezer', 'tidal', 'lastpass',
  'nordvpn', 'expressvpn', 'grammarly', 'evernote', 'todoist',
]);

// LEVEL 2 — generic signals: mark a transaction as subscription-related
// even without a known name. Combined with MCC filtering this catches
// unknown services that have 2+ recurring charges.
const GENERIC_SIGNALS = new Set([
  'subscription', 'premium', 'monthly', 'annual', 'membership',
  'підписка', 'преміум',
]);

// Ordered longest-first so substring scan picks the most specific name.
const KNOWN_ORDERED = [
  'microsoft', 'playstation', 'nordvpn', 'expressvpn', 'grammarly',
  'lastpass', 'duolingo', 'patreon', 'netflix', 'spotify',
  'youtube', 'dropbox', 'chatgpt', 'openai', 'disney', 'twitch',
  'icloud', 'amazon', 'claude', 'notion', 'canva', 'apple',
  'google', 'adobe', 'figma', 'slack', 'steam', 'hulu', 'zoom',
  'xbox', 'hbo', 'deezer', 'tidal', 'todoist', 'evernote',
];

exports.detect = async (userId, rawTransactions) => {
  const monobankService = require('./monobankService');
  await monobankService.saveTransactions(userId, rawTransactions);

  const allTx = await Transaction.find({ userId }).sort({ date: -1 });

  const groups = groupByService(allTx);
  const expenseTx = allTx.filter(t => t.amount < 0);
  console.log(`Detector: ${expenseTx.length} expense tx → ${groups.length} group(s)`);
  console.log("TX details:", expenseTx.map(t => `mcc=${t.mcc} counter="${t.counterName}" desc="${t.description}"`));
  console.log("Groups:", groups.map(g => `"${g.name}" tx=${g.transactions.length}`));

  let newCount = 0;
  let updatedCount = 0;

  for (const group of groups) {
    const { isKnown, hasGenericSignal } = classifyGroup(group);

    // Known service → always detect (even 1 transaction).
    // Unknown service → need 2+ transactions to avoid false positives.
    if (!isKnown && group.transactions.length < 2) continue;

    const interval = group.transactions.length >= 2
      ? detectInterval(group.transactions.map(t => t.date))
      : null;
    const resolvedInterval = interval ?? { cycle: 'monthly', days: null };

    const amountUah = Math.abs(group.amount) / 100;
    const existing = await Subscription.findOne({
      userId,
      name: group.name,
      amount: { $gte: amountUah * 0.9, $lte: amountUah * 1.1 },
    });

    const lastDate = group.transactions[0].date;
    const nextBillingDate = calcNextBillingDate(lastDate, resolvedInterval.cycle, resolvedInterval.days);

    if (!existing) {
      await Subscription.create({
        userId,
        name:            group.name,
        amount:          amountUah,
        currency:        'UAH',
        billingCycle:    resolvedInterval.cycle,
        customCycleDays: resolvedInterval.days,
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
  const allText = group.transactions
    .map(tx => `${tx.counterName || ''} ${tx.description || ''}`)
    .join(' ')
    .toLowerCase();

  const isKnown = [...KNOWN_SERVICES].some(kw => allText.includes(kw))
    || KNOWN_SERVICES.has(group.name.toLowerCase());

  const hasGenericSignal = [...GENERIC_SIGNALS].some(kw => allText.includes(kw));

  return { isKnown, hasGenericSignal };
}

function groupByService(transactions) {
  const groups = {};

  for (const tx of transactions) {
    if (tx.amount >= 0) continue;

    const byMcc = SUBSCRIPTION_MCC.includes(tx.mcc);
    const combined = `${tx.counterName || ''} ${tx.description || ''}`.toLowerCase();
    const byKnown = [...KNOWN_SERVICES].some(kw => combined.includes(kw));
    const bySignal = [...GENERIC_SIGNALS].some(kw => combined.includes(kw));

    if (!byMcc && !byKnown && !bySignal) continue;

    // Skip payment intermediaries (Xsolla, LiqPay, etc.) — they're not services.
    const name = extractName(tx.counterName, tx.description);
    if (PAYMENT_INTERMEDIARIES.has(name.toLowerCase())) continue;

    // For known services: group only by name (same service, different plan/price).
    // For unknown services: include amount in key to separate unrelated charges.
    const key = byKnown
      ? `known_${name.toLowerCase()}`
      : `${tx.mcc}_${Math.round(tx.amount / 100) * 100}_${name}`;

    if (!groups[key]) {
      groups[key] = { name, amount: tx.amount, mcc: tx.mcc, transactions: [] };
    } else {
      // Keep the most recent (largest absolute) amount for known services
      if (Math.abs(tx.amount) > Math.abs(groups[key].amount)) {
        groups[key].amount = tx.amount;
      }
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

  const counterWord = counterName.trim().split(/[\s,*]+/)[0];
  if (counterWord && counterWord.length > 1) return counterWord;
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