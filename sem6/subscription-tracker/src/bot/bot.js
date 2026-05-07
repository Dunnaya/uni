const { Telegraf } = require('telegraf');
const startCommand = require('./commands/start');
const subscriptionsCommand = require('./commands/subscriptions');
const upcomingCommand = require('./commands/upcoming');

let bot;

exports.startBot = () => {
  bot = new Telegraf(process.env.TELEGRAM_BOT_TOKEN);

  bot.command('start', startCommand);
  bot.command('subscriptions', subscriptionsCommand);
  bot.command('upcoming', upcomingCommand);

  bot.on('text', (ctx) => {
    ctx.reply(
      'Available commands:\n' +
      '/subscriptions — my subscriptions\n' +
      '/upcoming — upcoming charges'
    );
  });

  bot.launch();
  console.log('Telegram-bot started');

  process.once('SIGINT', () => bot.stop('SIGINT'));
  process.once('SIGTERM', () => bot.stop('SIGTERM'));
};

exports.getBot = () => bot;