const cron = require('node-cron');
const User = require('../models/User');
const Subscription = require('../models/Subscription');
const { getBot } = require('../bot/bot');

exports.startScheduler = () => {
  // start every day at 9:00 (jus like my fckin work =))))
  cron.schedule('0 9 * * *', async () => {
    console.log('Checking reminders...');
    await checkAndNotify();
  });
};

async function checkAndNotify() {
  const today = new Date();

  // all active subs with their renewal dates
  const subscriptions = await Subscription.find({ isActive: true });

  for (const sub of subscriptions) {
    const user = await User.findById(sub.userId);
    if (!user?.telegramLinked || !user.telegramChatId) continue;

    const reminderDays = sub.reminderDays ?? user.defaultReminderDays ?? 3;
    const targetDate = new Date(sub.nextBillingDate);
    targetDate.setHours(0, 0, 0, 0);

    const reminderDate = new Date(targetDate);
    reminderDate.setDate(reminderDate.getDate() - reminderDays);
    reminderDate.setHours(0, 0, 0, 0);

    today.setHours(0, 0, 0, 0);

    if (today.getTime() === reminderDate.getTime()) {
      const bot = getBot();
      const amount = (sub.amount / 100).toFixed(2);
      const dateStr = targetDate.toLocaleDateString('uk-UA');

      await bot.telegram.sendMessage(
        user.telegramChatId,
        `Reminder!!\n\n` +
        `Subscription *${sub.name}* will be charged ${dateStr}\n` +
        `Amount: ${amount} ${sub.currency}\n\n` +
        `To cancel the reminder — open the app.`,
        { parse_mode: 'Markdown' }
      );
    }
  }
}