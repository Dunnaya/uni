const User = require('../../models/User');

module.exports = async (ctx) => {
  try {
    const chatId = String(ctx.chat.id);
    const args = ctx.message.text.split(' ');

    if (args[1]) {
      const user = await User.findOne({ telegramLinkToken: args[1] });

      if (!user) {
        return ctx.reply('Invalid or expired linking token.\n\nGenerate a new one in the web app.');
      }

      if (!user.telegramLinkTokenExpiry || user.telegramLinkTokenExpiry < new Date()) {
        // clean up expired token
        user.telegramLinkToken = null;
        user.telegramLinkTokenExpiry = null;
        await user.save();
        return ctx.reply('This token has expired (valid for 15 minutes).\n\nGenerate a new one in the web app.');
      }

      user.telegramChatId = chatId;
      user.telegramLinked = true;
      user.telegramLinkToken = null;         // one-time — del after use
      user.telegramLinkTokenExpiry = null;
      await user.save();

      return ctx.reply(
        'Telegram linked to your account!\n\nAvailable commands:\n' +
        '/subscriptions — list your subscriptions\n' +
        '/upcoming — upcoming charges'
      );
    }

    ctx.reply(
      'Hi! I\'m your subscription tracker bot.\n\n' +
      'To link your account:\n' +
      '1. Log in to the web app\n' +
      '2. Go to Settings → Telegram\n' +
      '3. Click "Get link token" and copy the command\n' +
      '4. Send it here\n\n' +
      'Already linked? Try:\n' +
      '/subscriptions — your active subs\n' +
      '/upcoming — charges in the next 7 days'
    );
  } catch (err) {
    console.error('Bot /start error:', err.message);
    ctx.reply('Something went wrong. Please try again later.');
  }
};