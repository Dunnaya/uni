const router = require('express').Router();
const auth = require('../middleware/auth');
const forecastController = require('../controllers/forecastController');

router.get('/', auth, forecastController.getForecast);

module.exports = router;