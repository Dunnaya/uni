/**
 * Authentication controller functions
 * @module controllers/auth
 */

import User from "../models/user.model.js";

/**
 * Register a new user
 * @async
 * @function registerUser
 * @param {Object} req - Express request object
 * @param {Object} req.body - Request body
 * @param {string} req.body.name - User's name
 * @param {string} req.body.email - User's email
 * @param {string} req.body.password - User's password
 * @param {Object} res - Express response object
 * @returns {Object} JSON with success status, JWT token and user data
 */
export const registerUser = async (req, res) => {
  try {
    const { name, email, password } = req.body;

    const user = await User.create({
      name,
      email,
      password
    });

    const token = user.getSignedJwtToken();

    res.status(201).json({
      success: true,
      token,
      user: {
        id: user._id,
        name: user.name,
        email: user.email,
        role: user.role
      }
    });
  } catch (error) {
    res.status(400).json({
      success: false,
      message: error.message
    });
  }
};

/**
 * Login a user
 * @async
 * @function loginUser
 * @param {Object} req - Express request object
 * @param {Object} req.body - Request body
 * @param {string} req.body.email - User's email
 * @param {string} req.body.password - User's password
 * @param {Object} res - Express response object
 * @returns {Object} JSON with success status, JWT token and user data
 */
export const loginUser = async (req, res) => {
  try {
    const { email, password } = req.body;

    // validate email & password
    if (!email || !password) {
      return res.status(400).json({
        success: false,
        message: "Please provide an email and password"
      });
    }

    // check for user
    // IMPORTANT: need to explicitly select password field
    const user = await User.findOne({ email }).select('+password');

    if (!user) {
      return res.status(401).json({
        success: false,
        message: "Invalid credentials"
      });
    }

    // check if password matches
    const isMatch = await user.matchPassword(password);

    if (!isMatch) {
      return res.status(401).json({
        success: false,
        message: "Invalid credentials"
      });
    }

    const token = user.getSignedJwtToken();

    res.status(200).json({
      success: true,
      token,
      user: {
        id: user._id,
        name: user.name,
        email: user.email,
        role: user.role
      }
    });
  } catch (error) {
    res.status(500).json({
      success: false,
      message: error.message
    });
  }
};

/**
 * Get current logged in user
 * @async
 * @function getMe
 * @param {Object} req - Express request object
 * @param {Object} req.user - User object (set by auth middleware)
 * @param {Object} res - Express response object
 * @returns {Object} JSON with success status and user data
 */
export const getMe = async (req, res) => {
  try {
    const user = await User.findById(req.user.id);

    res.status(200).json({
      success: true,
      data: user
    });
  } catch (error) {
    res.status(500).json({
      success: false,
      message: error.message
    });
  }
};