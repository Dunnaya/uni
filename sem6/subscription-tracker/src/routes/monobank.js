const router = require('express').Router();
const ctrl = require('../controllers/monobankController');
const auth = require('../middleware/auth');
const rateLimit = require('../middleware/rateLimit');

router.use(auth);

// Sync is expensive: it can block up to several minutes for large date ranges
// (Monobank allows 1 request/60 s per account). Limit to 3 syncs per 10 min
// per IP to prevent accidental or deliberate hammering.
const syncLimit = rateLimit(3, 10 * 60 * 1000);

router.post('/token',  ctrl.saveToken);
router.delete('/token', ctrl.removeToken);
router.post('/sync',   syncLimit, ctrl.sync);
router.get('/status',  ctrl.status);

module.exports = router;