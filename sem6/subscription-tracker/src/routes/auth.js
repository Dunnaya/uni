const router = require('express').Router();
const authController = require('../controllers/authController');
const authMiddleware = require('../middleware/auth');
const rateLimit = require('../middleware/rateLimit');
const validate = require('../middleware/validate');

// Strict limits on auth endpoints to slow down brute-force and account enumeration.
const authLimit = rateLimit(10, 15 * 60 * 1000); // 10 attempts per 15 min per IP

router.post('/register', authLimit, validate.register, authController.register);
router.post('/login',    authLimit, validate.login,    authController.login);
router.get('/me',        authMiddleware,               authController.me);
router.post('/link-token', authMiddleware,             authController.generateLinkToken);

// Invalidates all existing JWTs for the authenticated user.
router.post('/revoke-tokens', authMiddleware, authController.revokeAllTokens);

module.exports = router;