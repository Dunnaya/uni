const jwt = require('jsonwebtoken');
const User = require('../models/User');

module.exports = async (req, res, next) => {
  const authHeader = req.headers.authorization;
  if (!authHeader?.startsWith('Bearer ')) {
    return res.status(401).json({ error: 'Authorization is missing' });
  }

  const token = authHeader.split(' ')[1];
  try {
    const decoded = jwt.verify(token, process.env.JWT_SECRET);

    const user = await User.findById(decoded.id).select('-passwordHash');
    if (!user) throw new Error('User not found');

    // Reject tokens that were issued with an older tokenVersion.
    // tokenVersion is incremented on logout-all or password change,
    // which invalidates all previously issued tokens for this user.
    if (decoded.tokenVersion !== user.tokenVersion) {
      return res.status(401).json({ error: 'Token has been revoked' });
    }

    req.user = user;
    next();
  } catch {
    res.status(401).json({ error: 'Invalid token' });
  }
};