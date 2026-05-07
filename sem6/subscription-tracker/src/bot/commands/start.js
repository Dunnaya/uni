const User = require('../../models/User');

module.exports = async (ctx) => {
  const chatId = String(ctx.chat.id);
  const args = ctx.message.text.split(' ');

  if (args[1]) {
    const user = await User.findOne({ telegramLinkToken: args[1] });
    if (user) {
      user.telegramChatId = chatId;
      user.telegramLinked = true;
      user.telegramLinkToken = undefined;
      await user.save();
      return ctx.reply('Telegram was linked to your account!');
    }
    return ctx.reply('Invalid or expired linking token.');
  }

  ctx.reply(
    'Hello! I am a bot for tracking subscriptions.\n' +
    'To link your account,[INSTRUCTIONS].'
  );
};