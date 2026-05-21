const { Telegraf } = require('telegraf');
const startCommand = require('./commands/start');
const subscriptionsCommand = require('./commands/subscriptions');
const upcomingCommand = require('./commands/upcoming');

let bot;

exports.startBot = () => {
  if (!process.env.TELEGRAM_BOT_TOKEN) {
    console.warn('TELEGRAM_BOT_TOKEN is not set — bot will not start');
    return;
  }

  bot = new Telegraf(process.env.TELEGRAM_BOT_TOKEN);

  bot.command('start', startCommand);
  bot.command('subscriptions', subscriptionsCommand);
  bot.command('upcoming', upcomingCommand);

  // catch-all for unrecognized messages
  bot.on('text', (ctx) => {
    ctx.reply(
      'Available commands:\n' +
      '/subscriptions — my subscriptions\n' +
      '/upcoming — upcoming charges'
    );
  });

  bot.launch();
  console.log('Telegram bot started');

  process.once('SIGINT', () => bot.stop('SIGINT'));
  process.once('SIGTERM', () => bot.stop('SIGTERM'));
};

exports.getBot = () => bot;