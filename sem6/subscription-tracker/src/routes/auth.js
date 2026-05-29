const router = require('express').Router();
const authController = require('../controllers/authController');
const authMiddleware = require('../middleware/auth');
const rateLimit = require('../middleware/rateLimit');
const validate = require('../middleware/validate');

const authLimit = rateLimit(10, 15 * 60 * 1000);

router.post('/register', authLimit, validate.register, authController.register);
router.post('/login',    authLimit, validate.login,    authController.login);
router.get('/me',        authMiddleware,               authController.me);
router.post('/link-token', authMiddleware,             authController.generateLinkToken);

router.post('/revoke-tokens', authMiddleware, authController.revokeAllTokens);

module.exports = router;