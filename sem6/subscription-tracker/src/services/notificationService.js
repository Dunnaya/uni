const cron = require('node-cron');
const User = require('../models/User');
const Subscription = require('../models/Subscription');
const { getBot } = require('../bot/bot');
const { calcNextBillingDate } = require('../utils/dateUtils');
const { escapeMd } = require('../bot/utils');

exports.startScheduler = () => {
  const hour   = process.env.NOTIFICATION_HOUR   || 9;
  const minute = process.env.NOTIFICATION_MINUTE || 0;

  cron.schedule(`${minute} ${hour} * * *`, async () => {
    console.log('Running daily scheduler...');
    try {
      await advancePastDates();
      await checkAndNotify();
    } catch (err) {
      console.error('Scheduler error:', err.message);
    }
  });
};

async function advancePastDates() {
  const today = new Date();
  today.setHours(0, 0, 0, 0);

  const outdated = await Subscription.find({
    isActive: true,
    nextBillingDate: { $lt: today },
  });

  for (const sub of outdated) {
    let next = new Date(sub.nextBillingDate);
    while (next < today) {
      next = calcNextBillingDate(next, sub.billingCycle, sub.customCycleDays);
    }
    sub.nextBillingDate = next;
    await sub.save();
  }

  if (outdated.length) {
    console.log(`Advanced ${outdated.length} outdated billing date(s)`);
  }
}

async function checkAndNotify() {
  const today = new Date();
  today.setHours(0, 0, 0, 0);

  const subscriptions = await Subscription.find({ isActive: true })
    .populate({ path: 'userId', select: 'telegramLinked telegramChatId defaultReminderDays' });

  const bot = getBot();
  if (!bot) return;

  for (const sub of subscriptions) {
    const user = sub.userId;
    if (!user?.telegramLinked || !user.telegramChatId) continue;

    const reminderDays = sub.reminderDays ?? user.defaultReminderDays ?? 3;

    const billingDate = new Date(sub.nextBillingDate);
    billingDate.setHours(0, 0, 0, 0);

    const notifyOn = new Date(billingDate);
    notifyOn.setDate(notifyOn.getDate() - reminderDays);

    if (today.getTime() !== notifyOn.getTime()) continue;

    // Escape user-controlled fields before embedding in MarkdownV2 message.
    const safeName = escapeMd(sub.name);
    const safeDate = escapeMd(billingDate.toLocaleDateString('uk-UA'));
    const safeAmount = escapeMd(sub.amount.toFixed(2));
    const safeCurrency = escapeMd(sub.currency);

    try {
      await bot.telegram.sendMessage(
        user.telegramChatId,
        `🔔 *Reminder*\n\n` +
        `*${safeName}* will be charged on ${safeDate}\n` +
        `Amount: ${safeAmount} ${safeCurrency}`,
        { parse_mode: 'MarkdownV2' }
      );
    } catch (err) {
      console.error(`Failed to notify user ${user._id}:`, err.message);
    }
  }
}

exports.advancePastDates = advancePastDates;