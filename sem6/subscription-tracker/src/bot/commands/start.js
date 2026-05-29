const User = require('../../models/User');

module.exports = async (ctx) => {
  try {
    const chatId = String(ctx.chat.id);
    const args = ctx.message.text.split(' ');

    if (args[1]) {
      const user = await User.findOne({ telegramLinkToken: args[1] });

      if (!user) {
        return ctx.reply('❌ Token not found or already used.\n\nGenerate a new one in the web app → Settings → Telegram bot.');
      }

      if (!user.telegramLinkTokenExpiry || user.telegramLinkTokenExpiry < new Date()) {
        user.telegramLinkToken = null;
        user.telegramLinkTokenExpiry = null;
        await user.save();
        return ctx.reply('❌ Token expired — it\'s valid for 15 minutes.\n\nGenerate a new one in the web app → Settings → Telegram bot.');
      }

      await User.updateOne(
        { telegramChatId: chatId, _id: { $ne: user._id } },
        { $set: { telegramChatId: null, telegramLinked: false } }
      );

      user.telegramChatId = chatId;
      user.telegramLinked = true;
      user.telegramLinkToken = null;
      user.telegramLinkTokenExpiry = null;
      await user.save();

      return ctx.reply(
        '✅ Account linked!\n\n' +
        'You\'ll get a reminder a few days before each charge.\n\n' +
        '/subscriptions — all active subscriptions\n' +
        '/upcoming — charges in the next 7 days'
      );
    }

    ctx.reply(
      'Hi! I\'ll remind you before your subscriptions renew 👋\n\n' +
      'To get started, link your account:\n' +
      '1. Open the web app\n' +
      '2. Go to Settings → Telegram bot\n' +
      '3. Click "Generate token" and send the command here\n\n' +
      'Already linked?\n' +
      '/subscriptions — your active subscriptions\n' +
      '/upcoming — charges in the next 7 days'
    );
  } catch (err) {
    console.error('Bot /start error:', err.message);
    ctx.reply('Something went wrong. Please try again later.');
  }
};