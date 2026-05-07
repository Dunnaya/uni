const User = require('../../models/User');
const Subscription = require('../../models/Subscription');
const { getUpcoming } = require('../../utils/dateUtils');

module.exports = async (ctx) => {
  const chatId = String(ctx.chat.id);
  const user = await User.findOne({ telegramChatId: chatId });

  if (!user) {
    return ctx.reply('Link your Telegram account in the web app first.');
  }

  const subs = await Subscription.find({ userId: user._id, isActive: true });
  const upcoming = getUpcoming(subs, 7);

  if (!upcoming.length) {
    return ctx.reply('✨ No upcoming charges in the next 7 days!');
  }

  const lines = upcoming.map(s => {
    const date = s.nextBillingDate.toLocaleDateString('uk-UA');
    const amount = (s.amount / 100).toFixed(2);
    return `📅 ${date} — ${s.name}: ${amount} ${s.currency}`;
  });

  ctx.reply('💳 No upcoming charges in the next 7 days!\n\n' + lines.join('\n'));
};