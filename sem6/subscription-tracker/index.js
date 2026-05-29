require('dotenv').config();

const REQUIRED_ENV = ['JWT_SECRET', 'CRYPTO_SECRET', 'MONGODB_URI'];
const missing = REQUIRED_ENV.filter(k => !process.env[k]);
if (missing.length) {
  console.error(`Missing required env variables: ${missing.join(', ')}`);
  process.exit(1);
}

const app = require('./src/app');
const connectDB = require('./src/config/db');
const { startBot } = require('./src/bot/bot');
const notificationService = require('./src/services/notificationService');

const PORT = process.env.PORT || 3000;

(async () => {
  await connectDB();

  app.listen(PORT, () => {
    console.log(`Server running on port ${PORT}`);
  });

  startBot();
  notificationService.startScheduler();

  await notificationService.advancePastDates();
})();