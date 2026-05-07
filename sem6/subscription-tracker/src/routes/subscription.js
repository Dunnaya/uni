const router = require('express').Router();
const ctrl = require('../controllers/subscriptionController');
const auth = require('../middleware/auth');

router.use(auth);   // all routes are protected

router.get('/', ctrl.getAll);
router.post('/', ctrl.create);
router.put('/:id', ctrl.update);
router.delete('/:id', ctrl.remove);
router.patch('/:id/toggle', ctrl.toggle);      // acivate/deactivate sub

module.exports = router;