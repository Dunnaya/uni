const Subscription = require('../models/Subscription');
const { calcNextBillingDate } = require('../utils/dateUtils');

// prognosis for upcom. N mo
exports.getForecast = async (userId, months = 3) => {
  const active = await Subscription.find({ userId, isActive: true });
  const now = new Date();
  const endDate = new Date();
  endDate.setMonth(endDate.getMonth() + months);

  const events = [];

  for (const sub of active) {
    let nextDate = new Date(sub.nextBillingDate);

    // generating all billing events for the period
    while (nextDate <= endDate) {
      if (nextDate >= now) {
        events.push({
          subscriptionId: sub._id,
          name: sub.name,
          amount: sub.amount,
          currency: sub.currency,
          date: new Date(nextDate),
          category: sub.category,
          isTrial: sub.isTrial,
        });
      }
      // next cycle
      nextDate = calcNextBillingDate(nextDate, sub.billingCycle, sub.customCycleDays);
    }
  }

  events.sort((a, b) => a.date - b.date);

  // monthly summary
  const monthlySummary = {};
  for (const e of events) {
    const key = `${e.date.getFullYear()}-${String(e.date.getMonth() + 1).padStart(2, '0')}`;
    if (!monthlySummary[key]) monthlySummary[key] = 0;
    monthlySummary[key] += e.amount;
  }

  return { events, monthlySummary };
};