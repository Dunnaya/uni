// next billing date based on cycle type
exports.calcNextBillingDate = (from, cycle, customDays = null) => {
  const date = new Date(from);

  switch (cycle) {
    case 'weekly':
      date.setDate(date.getDate() + 7);
      break;
    case 'monthly': {
      const day = date.getDate();
      date.setDate(1);
      date.setMonth(date.getMonth() + 1);
      const lastDay = new Date(date.getFullYear(), date.getMonth() + 1, 0).getDate();
      date.setDate(Math.min(day, lastDay));
      break;
    }
    case 'yearly': {
      const day = date.getDate();
      const month = date.getMonth();
      date.setFullYear(date.getFullYear() + 1);
      if (date.getMonth() !== month) {
        date.setDate(0);
      }
      break;
    }
    case 'custom':
      date.setDate(date.getDate() + (customDays || 30));
      break;
    default:
      date.setDate(date.getDate() + 30);
  }

  return date;
};

exports.getUpcoming = (subscriptions, days = 7) => {
  const now = new Date();
  const limit = new Date(now.getTime() + days * 24 * 60 * 60 * 1000);
  return subscriptions
    .filter(s => s.nextBillingDate >= now && s.nextBillingDate <= limit)
    .sort((a, b) => a.nextBillingDate - b.nextBillingDate);  // ← додати це
};