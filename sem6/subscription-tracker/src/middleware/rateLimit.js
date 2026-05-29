// in-memory rate limiter
// for multi-instance production deployments replace with Redis (e.g. rate-limiter-flexible)

const buckets = new Map();

const PRUNE_INTERVAL = 10 * 60 * 1000;
setInterval(() => {
  const cutoff = Date.now() - 60 * 60 * 1000;
  for (const [key, timestamps] of buckets) {
    if (!timestamps.length || timestamps[timestamps.length - 1] < cutoff) {
      buckets.delete(key);
    }
  }
}, PRUNE_INTERVAL).unref();

module.exports = (maxAttempts = 10, windowMs = 15 * 60 * 1000) => (req, res, next) => {
  const key = req.ip;
  const now = Date.now();
  const windowStart = now - windowMs;

  const recent = (buckets.get(key) || []).filter(t => t > windowStart);

  if (recent.length >= maxAttempts) {
    return res.status(429).json({ error: 'Too many attempts, please try again later' });
  }

  recent.push(now);
  buckets.set(key, recent);
  next();
};