const Transaction = require('../models/Transaction');
const Subscription = require('../models/Subscription');

const SUBSCRIPTION_MCC = [5815, 5817, 7372, 4816, 4899, 7995, 5968];

const NON_SUBSCRIPTION_NAMES = new Set([
  'xsolla', 'liqpay', 'fondy', 'wayforpay', 'portmone', 'easypay', 'ipay', 'paypal', 'stripe',
  'steam', 'epic', 'gog', 'origin', 'ubisoft',
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

  const importedExternalIds = new Set(rawTransactions.map(t => t.id));
  const importedTx = await Transaction.find({
    userId,
    externalId: { $in: [...importedExternalIds] },
  }).sort({ date: -1 });

  if (!importedTx.length) return { new: 0, updated: 0 };

  const importGroups = groupByService(importedTx);
  const enrichedGroups = await enrichWithHistory(userId, importGroups, importedExternalIds);

  let newCount = 0;
  let updatedCount = 0;

  for (const group of enrichedGroups) {
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

    const mostRecentDate = group.transactions[0].date;
    const oldestDate = group.transactions[group.transactions.length - 1].date;
    const nextBillingDate = calcNextBillingDateWithDay(
      mostRecentDate, cycle, days, oldestDate.getDate()
    );

    if (!existing) {
      await Subscription.create({
        userId,
        name:            group.name,
        amount:          amountUah,
        currency:        'UAH',
        billingCycle:    cycle,
        customCycleDays: days,
        nextBillingDate,
        startDate:       oldestDate,
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

async function enrichWithHistory(userId, groups, importedExternalIds) {
  const enriched = [];

  for (const group of groups) {
    const historical = await Transaction.find({
      userId,
      externalId: { $nin: [...importedExternalIds] },
      $or: [
        { counterName: new RegExp(group.name, 'i') },
        { description: new RegExp(group.name, 'i') },
      ],
    }).sort({ date: -1 });

    const allTx = [...group.transactions];
    for (const htx of historical) {
      const alreadyIn = allTx.some(t => String(t._id) === String(htx._id));
      if (!alreadyIn) allTx.push(htx);
    }

    enriched.push({ ...group, transactions: allTx });
  }

  return enriched;
}

function classifyGroup(group) {
  const text = group.transactions
    .map(tx => `${tx.counterName || ''} ${tx.description || ''}`)
    .join(' ')
    .toLowerCase();
  return [...KNOWN_SERVICES].some(kw => text.includes(kw))
    || KNOWN_SERVICES.has(group.name.toLowerCase());
}

function groupByService(transactions) {
  const groups = {};
  const refundedIds = buildRefundedChargeSet(transactions);

  for (const tx of transactions) {
    if (tx.amount >= 0) continue;

    const combined = `${tx.counterName || ''} ${tx.description || ''}`.toLowerCase();
    const byMcc    = SUBSCRIPTION_MCC.includes(tx.mcc);
    const byKnown  = [...KNOWN_SERVICES].some(kw => combined.includes(kw));
    const bySignal = [...GENERIC_SIGNALS].some(kw => combined.includes(kw));

    if (!byMcc && !byKnown && !bySignal) continue;

    const name = extractName(tx.counterName, tx.description);
    if (NON_SUBSCRIPTION_NAMES.has(name.toLowerCase())) continue;

    if (refundedIds.has(String(tx._id))) continue;

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

function buildRefundedChargeSet(transactions) {
  const refundedIds = new Set();
  const charges = transactions.filter(t => t.amount < 0);
  const refunds  = transactions.filter(t => t.amount > 0);

  for (const refund of refunds) {
    const refundName = extractName(refund.counterName, refund.description).toLowerCase();
    for (const charge of charges) {
      if (extractName(charge.counterName, charge.description).toLowerCase() !== refundName) continue;
      if (Math.abs(charge.amount) !== refund.amount) continue;
      const diffH = Math.abs(new Date(refund.date) - new Date(charge.date)) / 36e5;
      if (diffH <= 48) refundedIds.add(String(charge._id));
    }
  }

  return refundedIds;
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
    diffs.push((sorted[i] - sorted[i - 1]) / 864e5);
  }
  if (!diffs.length) return { cycle: 'monthly', days: null };
  const avg = diffs.reduce((a, b) => a + b, 0) / diffs.length;
  if (avg < 1)   return { cycle: 'monthly', days: null };
  if (avg < 9)   return { cycle: 'weekly',  days: null };
  if (avg < 100) return { cycle: 'monthly', days: null };
  if (avg < 400) return { cycle: 'yearly',  days: null };
  return { cycle: 'custom', days: Math.round(avg) };
}

function calcNextBillingDateWithDay(from, cycle, days, billingDay) {
  const { calcNextBillingDate } = require('../utils/dateUtils');

  let result;

  if (cycle === 'monthly') {
    const base = new Date(from);
    base.setDate(1);
    base.setMonth(base.getMonth() + 1);
    const lastDay = new Date(base.getFullYear(), base.getMonth() + 1, 0).getDate();
    base.setDate(Math.min(billingDay, lastDay));
    result = base;
  } else if (cycle === 'yearly') {
    const base = new Date(from);
    const origMonth = base.getMonth();
    base.setFullYear(base.getFullYear() + 1);
    if (base.getMonth() !== origMonth) base.setDate(0);
    const lastDay = new Date(base.getFullYear(), base.getMonth() + 1, 0).getDate();
    base.setDate(Math.min(billingDay, lastDay));
    result = base;
  } else {
    result = calcNextBillingDate(from, cycle, days);
  }

  const now = new Date();
  while (result <= now) {
    result = calcNextBillingDateWithDay(result, cycle, days, billingDay);
  }

  return result;
}