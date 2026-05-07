/**
 * Calculates the next billing date based on the cycle type
 */
exports.calcNextBillingDate = (from, cycle, customDays = null) => {
  const date = new Date(from);

  switch (cycle) {
    case 'weekly':
      date.setDate(date.getDate() + 7);
      break;
    case 'monthly':
      date.setMonth(date.getMonth() + 1);
      break;
    case 'yearly':
      date.setFullYear(date.getFullYear() + 1);
      break;
    case 'custom':
      date.setDate(date.getDate() + (customDays || 30));
      break;
    default:
      date.setMonth(date.getMonth() + 1);
  }

  return date;
};

/**
 * Returns subscriptions that are due for billing within the next N days
 */
exports.getUpcoming = (subscriptions, days = 7) => {
  const now = new Date();
  const limit = new Date(now.getTime() + days * 24 * 60 * 60 * 1000);
  return subscriptions.filter(s => s.nextBillingDate >= now && s.nextBillingDate <= limit);
};