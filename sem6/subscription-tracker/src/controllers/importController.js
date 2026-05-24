const csvParser = require('../utils/csvParser');
const subscriptionDetector = require('../services/subscriptionDetector');
const Transaction = require('../models/Transaction');
const crypto = require('crypto');

// POST /api/import — multipart/form-data, fields: file, bank (optional)
exports.importFile = async (req, res, next) => {
  try {
    if (!req.file) {
      return res.status(400).json({ error: 'No file uploaded' });
    }

    const bank = req.body.bank || 'generic';
    const parsed = csvParser.parseFile(req.file.buffer, req.file.originalname, bank);

    if (!parsed.length) {
      return res.status(422).json({ error: 'No valid transactions found in the file' });
    }

    // reshape to match monobank tx format so detector can reuse the same logic
    const transactions = parsed.map((row, i) => ({
      id: `csv_${crypto.createHash('md5').update(`${row.date}${row.amount}${row.description}${i}`).digest('hex')}`,
      time: Math.floor(new Date(row.date).getTime() / 1000),
      amount: row.amount,
      description: row.description || '',
      // counterName is the enriched merchant name (e.g. "Google YouTube" vs generic "Оплата в інтернеті").
      // Without it, the subscription detector can't match Google/Apple services from CSV imports.
      counterName: row.counterName || row.description || '',
      mcc: row.mcc || 0,
      currencyCode: 980,
    }));

    // save to DB and detect subs
    const detected = await subscriptionDetector.detect(req.user._id, transactions);

    res.json({
      parsed: parsed.length,
      newSubscriptions: detected.new,
      updatedSubscriptions: detected.updated,
    });
  } catch (err) {
    next(err);
  }
};