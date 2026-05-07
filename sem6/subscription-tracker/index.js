require('dotenv').config();
const app = require('./src/app');
const connectDB = require('./src/config/db');
const { startBot } = require('./src/bot/bot');
const notificationService = require('./src/services/notificationService');

const PORT = process.env.PORT || 3000;

(async () => {
  await connectDB();

  // start Express
  app.listen(PORT, () => {
    console.log(`🚀 Сервер запущено на порту ${PORT}`);
  });

  // start Tg bot
  startBot();

  // start notification scheduler
  notificationService.startScheduler();
})();