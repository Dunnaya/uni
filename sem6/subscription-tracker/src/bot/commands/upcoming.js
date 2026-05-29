const User = require('../../models/User');
const Subscription = require('../../models/Subscription');
const { getUpcoming } = require('../../utils/dateUtils');
const { escapeMd } = require('../utils');

module.exports = async (ctx) => {
  try {
    const chatId = String(ctx.chat.id);
    const user = await User.findOne({ telegramChatId: chatId });

    if (!user) {
      return ctx.reply(
        '⚠️ Your Telegram account isn\'t linked yet.\nSend /start to see how.'
      );
    }

    const subs = await Subscription.find({ userId: user._id, isActive: true });
    const upcoming = getUpcoming(subs, 7);

    if (!upcoming.length) {
      return ctx.reply('✅ No charges in the next 7 days — enjoy the quiet!');
    }

    const lines = upcoming.map(s => {
      const date   = escapeMd(s.nextBillingDate.toLocaleDateString('uk-UA'));
      const name   = escapeMd(s.name);
      const amount   = escapeMd(s.amount.toFixed(2));
      const currency = escapeMd(s.currency);
      return `${date} — *${name}*: ${amount} ${currency}`;
    });

    ctx.reply(
      `*Upcoming charges — next 7 days:*\n\n` + lines.join('\n'),
      { parse_mode: 'MarkdownV2' }
    );
  } catch (err) {
    console.error('Bot /upcoming error:', err.message);
    ctx.reply('Something went wrong. Please try again later.');
  }
};