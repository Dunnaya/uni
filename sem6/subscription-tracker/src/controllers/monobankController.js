const cryptoService = require('../services/cryptoService');
const monobankService = require('../services/monobankService');
const subscriptionDetector = require('../services/subscriptionDetector');
const User = require('../models/User');

exports.saveToken = async (req, res, next) => {
  try {
    const { token } = req.body;
    if (!token) return res.status(400).json({ error: 'Token is missing' });

    // verify against monobank API before saving
    await monobankService.verifyToken(token);

    // encrypt and save
    req.user.monobankToken = cryptoService.encrypt(token);
    await req.user.save();

    res.json({ message: 'Token saved' });
  } catch (err) { next(err); }
};

exports.removeToken = async (req, res, next) => {
  try {
    req.user.monobankToken = null;
    await req.user.save();
    res.json({ message: 'Token removed' });
  } catch (err) { next(err); }
};

exports.sync = async (req, res, next) => {
  try {
    if (!req.user.monobankToken) {
      return res.status(400).json({ error: 'Monobank token is not configured' });
    }

    const rawToken = cryptoService.decrypt(req.user.monobankToken);

    const from = new Date(Date.now() - 90 * 24 * 60 * 60 * 1000);
    const transactions = await monobankService.fetchTransactions(rawToken, from);
    const detected = await subscriptionDetector.detect(req.user._id, transactions);

    req.user.monobankLastSync = new Date();
    await req.user.save();

    res.json({
      synced: transactions.length,
      newSubscriptions: detected.new,
      updatedSubscriptions: detected.updated,
    });
  } catch (err) { next(err); }
};

exports.status = (req, res) => {
  res.json({
    connected: !!req.user.monobankToken,
    lastSync: req.user.monobankLastSync,
  });
};