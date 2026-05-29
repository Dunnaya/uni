const Subscription = require('../models/Subscription');
const { calcNextBillingDate } = require('../utils/dateUtils');

exports.getForecast = async (userId, months = 3) => {
  const active = await Subscription.find({ userId, isActive: true });
  const now = new Date();
  const endDate = new Date();
  endDate.setMonth(endDate.getMonth() + months);

  const events = [];

  for (const sub of active) {
    let nextDate = new Date(sub.nextBillingDate);
    
    while (nextDate < now) {
      nextDate = calcNextBillingDate(nextDate, sub.billingCycle, sub.customCycleDays);
    }

    while (nextDate <= endDate) {
      events.push({
        subscriptionId: sub._id,
        name:     sub.name,
        amount:   sub.amount,
        currency: sub.currency,
        date:     new Date(nextDate),
        category: sub.category,
        isTrial:  sub.isTrial,
      });
      nextDate = calcNextBillingDate(nextDate, sub.billingCycle, sub.customCycleDays);
    }
  }

  events.sort((a, b) => a.date - b.date);

  const monthlySummary = {};
  for (const e of events) {
    const monthKey = `${e.date.getFullYear()}-${String(e.date.getMonth() + 1).padStart(2, '0')}`;
    if (!monthlySummary[monthKey]) monthlySummary[monthKey] = {};
    const cur = e.currency || 'UAH';
    monthlySummary[monthKey][cur] = (monthlySummary[monthKey][cur] || 0) + e.amount;
  }

  return { events, monthlySummary };
};