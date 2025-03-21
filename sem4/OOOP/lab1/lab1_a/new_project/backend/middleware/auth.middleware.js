/**
 * Authentication middleware functions
 * @module middleware/auth
 */

import jwt from "jsonwebtoken";
import User from "../models/user.model.js";

/**
 * Middleware to protect routes - requires authentication
 * Checks JWT token and attaches user to request object
 * @async
 * @function protect
 * @param {Object} req - Express request object
 * @param {Object} res - Express response object
 * @param {Function} next - Express next middleware function
 * @returns {void}
 */
export const protect = async (req, res, next) => {
  let token;

  // check for token in authorization header
  if (
    req.headers.authorization &&
    req.headers.authorization.startsWith("Bearer")
  ) {
    // get token from header
    token = req.headers.authorization.split(" ")[1];
  }

  // check if token exists
  if (!token) {
    return res.status(401).json({
      success: false,
      message: "Not authorized to access this route",
    });
  }

  try {
    // verify token
    const decoded = jwt.verify(token, process.env.JWT_SECRET);

    // get user from the token
    req.user = await User.findById(decoded.id);

    next();
  } catch (error) {
    return res.status(401).json({
      success: false,
      message: "Not authorized to access this route",
    });
  }
};

/**
 * Middleware for role-based access control
 * @function authorize
 * @param {...string} roles - Roles allowed to access the route
 * @returns {Function} Express middleware function
 */
export const authorize = (...roles) => {
  return (req, res, next) => {
    if (!roles.includes(req.user.role)) {
      return res.status(403).json({
        success: false,
        message: `User role "${req.user.role}" is not authorized to access this route`,
      });
    }
    next();
  };
};