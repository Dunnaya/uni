const router = require('express').Router();
const ctrl = require('../controllers/monobankController');
const auth = require('../middleware/auth');
const rateLimit = require('../middleware/rateLimit');

router.use(auth);

const syncLimit = rateLimit(3, 10 * 60 * 1000);

router.post('/token',  ctrl.saveToken);
router.delete('/token', ctrl.removeToken);
router.post('/sync',   syncLimit, ctrl.sync);
router.get('/status',  ctrl.status);

module.exports = router;