// In-memory rate limiter.
// For multi-instance production deployments replace with Redis (e.g. rate-limiter-flexible).

const buckets = new Map();

// Prune entries older than the window every PRUNE_INTERVAL ms
// to prevent unbounded Map growth (memory leak on long-running servers).
const PRUNE_INTERVAL = 10 * 60 * 1000; // 10 min
setInterval(() => {
  const cutoff = Date.now() - 60 * 60 * 1000; // drop buckets silent for 1 h
  for (const [key, timestamps] of buckets) {
    if (!timestamps.length || timestamps[timestamps.length - 1] < cutoff) {
      buckets.delete(key);
    }
  }
}, PRUNE_INTERVAL).unref(); // .unref() so the timer doesn't keep the process alive

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