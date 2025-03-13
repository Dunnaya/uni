import jwt from "jsonwebtoken";
import User from "../models/user.model.js";

// middleware to protect routes - require authentication
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

// middleware for role-based access control
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