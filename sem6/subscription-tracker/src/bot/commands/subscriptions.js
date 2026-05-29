const User = require('../../models/User');
const Subscription = require('../../models/Subscription');
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

    const subs = await Subscription.find({ userId: user._id, isActive: true })
      .sort({ nextBillingDate: 1 });

    if (!subs.length) {
      return ctx.reply('No active subscriptions yet — add some in the web app!');
    }

    const cycleLabel = { weekly: 'week', monthly: 'month', yearly: 'year', custom: 'custom' };

    const lines = subs.map(s => {
      const name   = escapeMd(s.name);
      const amount   = escapeMd(s.amount.toFixed(2));
      const currency = escapeMd(s.currency);
      const cycle    = escapeMd(cycleLabel[s.billingCycle] || s.billingCycle);
      const trial    = s.isTrial ? ' _(trial)_' : '';
      return `• *${name}* — ${amount} ${currency}/${cycle}${trial}`;
    });

    ctx.reply(
      `*Active subscriptions \\(${subs.length}\\):*\n\n` + lines.join('\n'),
      { parse_mode: 'MarkdownV2' }
    );
  } catch (err) {
    console.error('Bot /subscriptions error:', err.message);
    ctx.reply('Something went wrong. Please try again later.');
  }
};