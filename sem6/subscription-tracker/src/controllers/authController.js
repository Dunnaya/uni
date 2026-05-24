const jwt = require('jsonwebtoken');
const crypto = require('crypto');
const User = require('../models/User');

const generateToken = (user) =>
  jwt.sign(
    { id: user._id, tokenVersion: user.tokenVersion },
    process.env.JWT_SECRET,
    { expiresIn: '30d' }
  );

exports.register = async (req, res, next) => {
  try {
    const { email, password } = req.body;
    const existing = await User.findOne({ email });
    if (existing) return res.status(409).json({ error: 'Email is already registered' });

    const user = await User.create({ email, passwordHash: password });
    res.status(201).json({ token: generateToken(user) });
  } catch (err) {
    next(err);
  }
};

exports.login = async (req, res, next) => {
  try {
    const { email, password } = req.body;
    const user = await User.findOne({ email });

    if (!user || !(await user.comparePassword(password))) {
      return res.status(401).json({ error: 'Invalid credentials' });
    }
    res.json({ token: generateToken(user) });
  } catch (err) {
    next(err);
  }
};

exports.me = (req, res) => res.json(req.user);

exports.revokeAllTokens = async (req, res, next) => {
  try {
    req.user.tokenVersion = (req.user.tokenVersion || 0) + 1;
    await req.user.save();
    res.json({ message: 'All tokens revoked. Please log in again.' });
  } catch (err) {
    next(err);
  }
};

exports.generateLinkToken = async (req, res, next) => {
  try {
    const token = crypto.randomBytes(16).toString('hex');

    req.user.telegramLinkToken = token;
    req.user.telegramLinkTokenExpiry = new Date(Date.now() + 15 * 60 * 1000);
    await req.user.save();

    res.json({
      token,
      instruction: `Send this command to the bot: /start ${token}`,
    });
  } catch (err) {
    next(err);
  }
};