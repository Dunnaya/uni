// in-memory, good enough for now (for prod would use Redis)

const attempts = new Map();

module.exports = (maxAttempts = 10, windowMs = 15 * 60 * 1000) => (req, res, next) => {
  const key = req.ip;
  const now = Date.now();
  const windowStart = now - windowMs;

  const recent = (attempts.get(key) || []).filter(t => t > windowStart);

  if (recent.length >= maxAttempts) {
    return res.status(429).json({ error: 'Too many attempts, please try again later' });
  }

  recent.push(now);
  attempts.set(key, recent);
  next();
};