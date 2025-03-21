/**
 * User routes
 * @module routes/user
 */

import express from "express";
import { registerUser, loginUser, getMe } from "../controllers/auth.controller.js";
import { protect, authorize } from "../middleware/auth.middleware.js";

const router = express.Router();

/**
 * @route POST /api/users/register
 * @desc Register a new user
 * @access Public
 */
router.post("/register", registerUser);

/**
 * @route POST /api/users/login
 * @desc Login user and get token
 * @access Public
 */
router.post("/login", loginUser);

/**
 * @route GET /api/users/me
 * @desc Get current user profile
 * @access Private
 */
router.get("/me", protect, getMe);

export default router;