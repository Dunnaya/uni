// next billing date based on cycle type
exports.calcNextBillingDate = (from, cycle, customDays = null) => {
  const date = new Date(from);

  switch (cycle) {
    case 'weekly':
      date.setDate(date.getDate() + 7);
      break;
    case 'monthly': {
      // JS setMonth overflows: Jan 31 + 1 month = Mar 3, not Feb 28.
      // Clamp to the last day of the target month instead.
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
      // Handle Feb 29 in leap year -> Feb 28 in non-leap year
      if (date.getMonth() !== month) {
        date.setDate(0); // last day of previous month
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

// subscriptions due within the next N days
exports.getUpcoming = (subscriptions, days = 7) => {
  const now = new Date();
  const limit = new Date(now.getTime() + days * 24 * 60 * 60 * 1000);
  return subscriptions.filter(s => s.nextBillingDate >= now && s.nextBillingDate <= limit);
};