const router = require('express').Router();
const ctrl = require('../controllers/monobankController');
const auth = require('../middleware/auth');

router.use(auth);

router.post('/token', ctrl.saveToken);       // save/upd token
router.delete('/token', ctrl.removeToken);   // del token
router.post('/sync', ctrl.sync);             // sync transactions
router.get('/status', ctrl.status);          // connection status

module.exports = router;