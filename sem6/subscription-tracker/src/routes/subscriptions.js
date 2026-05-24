const router = require('express').Router();
const ctrl = require('../controllers/subscriptionController');
const auth = require('../middleware/auth');
const validate = require('../middleware/validate');

router.use(auth);

router.get('/', ctrl.getAll);
router.post('/', validate.subscription, ctrl.create);
router.put('/:id', validate.subscriptionUpdate, ctrl.update);
router.delete('/:id', ctrl.remove);
router.patch('/:id/toggle', ctrl.toggle);

module.exports = router;