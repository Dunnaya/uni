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
        'Link your Telegram account in the web app first.\nSend /start to see instructions.'
      );
    }

    const subs = await Subscription.find({ userId: user._id, isActive: true });
    const upcoming = getUpcoming(subs, 7);

    if (!upcoming.length) {
      return ctx.reply('No upcoming charges in the next 7 days!');
    }

    const lines = upcoming.map(s => {
      const date   = escapeMd(s.nextBillingDate.toLocaleDateString('uk-UA'));
      const name   = escapeMd(s.name);
      const amount = s.amount.toFixed(2);
      return `${date} — *${name}*: ${amount} ${s.currency}`;
    });

    ctx.reply(
      `💳 *Upcoming charges \\(next 7 days\\):*\n\n` + lines.join('\n'),
      { parse_mode: 'MarkdownV2' }
    );
  } catch (err) {
    console.error('Bot /upcoming error:', err.message);
    ctx.reply('Something went wrong. Please try again later.');
  }
};