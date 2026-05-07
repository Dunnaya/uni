const Subscription = require('../models/Subscription');
const { calcNextBillingDate } = require('../utils/dateUtils');

exports.getAll = async (req, res, next) => {
  try {
    const { active, category } = req.query;
    const filter = { userId: req.user._id };
    if (active !== undefined) filter.isActive = active === 'true';
    if (category) filter.category = category;

    const subscriptions = await Subscription.find(filter).sort({ nextBillingDate: 1 });
    res.json(subscriptions);
  } catch (err) { next(err); }
};

exports.create = async (req, res, next) => {
  try {
    const data = {
      ...req.body,
      userId: req.user._id,
      source: 'manual',
    };

    // if nextBillingDate not provided, calculate auto
    if (!data.nextBillingDate && data.startDate) {
      data.nextBillingDate = calcNextBillingDate(
        new Date(data.startDate),
        data.billingCycle,
        data.customCycleDays
      );
    }

    const sub = await Subscription.create(data);
    res.status(201).json(sub);
  } catch (err) { next(err); }
};

exports.update = async (req, res, next) => {
  try {
    const sub = await Subscription.findOneAndUpdate(
      { _id: req.params.id, userId: req.user._id },
      req.body,
      { new: true, runValidators: true }
    );
    if (!sub) return res.status(404).json({ error: 'Subscription not found' });
    res.json(sub);
  } catch (err) { next(err); }
};

exports.remove = async (req, res, next) => {
  try {
    await Subscription.findOneAndDelete({ _id: req.params.id, userId: req.user._id });
    res.json({ message: 'Subscription removed' });
  } catch (err) { next(err); }
};

exports.toggle = async (req, res, next) => {
  try {
    const sub = await Subscription.findOne({ _id: req.params.id, userId: req.user._id });
    if (!sub) return res.status(404).json({ error: 'Subscription not found' });
    sub.isActive = !sub.isActive;
    await sub.save();
    res.json(sub);
  } catch (err) { next(err); }
};