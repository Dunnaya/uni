const router = require('express').Router();
const authController = require('../controllers/authController');
const authMiddleware = require('../middleware/auth');
const rateLimit = require('../middleware/rateLimit');
const validate = require('../middleware/validate');

router.post('/register', validate.register, authController.register);
router.post('/login', rateLimit(10, 15 * 60 * 1000), validate.login, authController.login);
router.get('/me', authMiddleware, authController.me);
router.post('/link-token', authMiddleware, authController.generateLinkToken);

module.exports = router;