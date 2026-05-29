const EMAIL_RE = /^[^\s@]+@[^\s@]+\.[^\s@]+$/;
const VALID_CYCLES = ['weekly', 'monthly', 'yearly', 'custom'];

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
  const { name, amount, billingCycle, nextBillingDate, reminderDays, customCycleDays } = req.body;

  if (!name || amount === undefined || !billingCycle || !nextBillingDate)
    return res.status(400).json({ error: 'name, amount, billingCycle and nextBillingDate are required' });

  if (typeof amount !== 'number' || amount <= 0)
    return res.status(400).json({ error: 'amount must be a positive number' });

  if (!VALID_CYCLES.includes(billingCycle))
    return res.status(400).json({ error: `billingCycle must be one of: ${VALID_CYCLES.join(', ')}` });

  if (billingCycle === 'custom') {
    if (!customCycleDays || typeof customCycleDays !== 'number' || customCycleDays < 1)
      return res.status(400).json({ error: 'customCycleDays is required and must be a positive number when billingCycle is custom' });
  }

  if (reminderDays !== undefined) {
    if (typeof reminderDays !== 'number' || !Number.isInteger(reminderDays) || reminderDays < 0)
      return res.status(400).json({ error: 'reminderDays must be a non-negative integer' });
  }

  if (isNaN(Date.parse(nextBillingDate)))
    return res.status(400).json({ error: 'nextBillingDate must be a valid date' });

  next();
};

exports.subscriptionUpdate = (req, res, next) => {
  const { amount, billingCycle, nextBillingDate, reminderDays, customCycleDays } = req.body;

  if (amount !== undefined && (typeof amount !== 'number' || amount <= 0))
    return res.status(400).json({ error: 'amount must be a positive number' });

  if (billingCycle !== undefined && !VALID_CYCLES.includes(billingCycle))
    return res.status(400).json({ error: `billingCycle must be one of: ${VALID_CYCLES.join(', ')}` });

  if (billingCycle === 'custom') {
    if (!customCycleDays || typeof customCycleDays !== 'number' || customCycleDays < 1)
      return res.status(400).json({ error: 'customCycleDays is required and must be a positive number when billingCycle is custom' });
  }

  if (reminderDays !== undefined && reminderDays !== null) {
    if (typeof reminderDays !== 'number' || !Number.isInteger(reminderDays) || reminderDays < 0)
      return res.status(400).json({ error: 'reminderDays must be a non-negative integer' });
  }

  if (nextBillingDate !== undefined && isNaN(Date.parse(nextBillingDate)))
    return res.status(400).json({ error: 'nextBillingDate must be a valid date' });

  next();
};