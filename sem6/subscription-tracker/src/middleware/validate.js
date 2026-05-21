const EMAIL_RE = /^[^\s@]+@[^\s@]+\.[^\s@]+$/;

exports.register = (req, res, next) => {
  const { email, password } = req.body;
  if (!email || !password)
    return res.status(400).json({ error: 'Email and password are required' });
  if (!EMAIL_RE.test(email))
    return res.status(400).json({ error: 'Invalid email format' });
  if (password.length < 6)
    return res.status(400).json({ error: 'Password must be at least 6 characters' });
  next();
};

exports.login = (req, res, next) => {
  const { email, password } = req.body;
  if (!email || !password)
    return res.status(400).json({ error: 'Email and password are required' });
  next();
};

exports.subscription = (req, res, next) => {
  const { name, amount, billingCycle, nextBillingDate } = req.body;
  if (!name || amount === undefined || !billingCycle || !nextBillingDate)
    return res.status(400).json({ error: 'name, amount, billingCycle and nextBillingDate are required' });
  if (typeof amount !== 'number' || amount <= 0)
    return res.status(400).json({ error: 'amount must be a positive number' });
  const VALID_CYCLES = ['weekly', 'monthly', 'yearly', 'custom'];
  if (!VALID_CYCLES.includes(billingCycle))
    return res.status(400).json({ error: `billingCycle must be one of: ${VALID_CYCLES.join(', ')}` });
  next();
};