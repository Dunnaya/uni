const csvParser = require('../utils/csvParser');
const subscriptionDetector = require('../services/subscriptionDetector');
const crypto = require('crypto');

exports.importFile = async (req, res, next) => {
  try {
    if (!req.file) {
      return res.status(400).json({ error: 'No file uploaded' });
    }

    const parsed = csvParser.parseFile(req.file.buffer, req.file.originalname);

    if (!parsed.length) {
      return res.status(422).json({ error: 'No valid transactions found in the file' });
    }

    const transactions = parsed.map(row => ({
      id: `csv_${crypto.createHash('md5').update(`${row.date}|${row.amount}|${row.description}`).digest('hex')}`,
      time: Math.floor(new Date(row.date).getTime() / 1000),
      amount: row.amount,
      description: row.description || '',
      counterName: row.counterName || row.description || '',
      mcc: row.mcc || 0,
      currencyCode: 980,
      source: 'csv',
    }));

    const detected = await subscriptionDetector.detect(req.user._id, transactions);

    const expenseCount = parsed.filter(r => r.amount < 0).length;

    res.json({
      parsed: expenseCount,
      newSubscriptions: detected.new,
      updatedSubscriptions: detected.updated,
    });
  } catch (err) {
    next(err);
  }
};