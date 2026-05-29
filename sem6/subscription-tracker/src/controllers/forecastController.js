const forecastService = require('../services/forecastService');

exports.getForecast = async (req, res, next) => {
  try {
    const months = parseInt(req.query.months) || 3;

    if (months < 1 || months > 24) {
      return res.status(400).json({ error: 'months must be between 1 and 24' });
    }

    const forecast = await forecastService.getForecast(req.user._id, months);
    res.json(forecast);
  } catch (err) {
    next(err);
  }
};