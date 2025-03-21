/**
 * Main application entry point
 * @module server
 */

import express from "express";
import dotenv from "dotenv";
import path from "path";

import { connectDB } from "./config/db.js";

import productRoutes from "./routes/product.route.js";
import userRoutes from "./routes/user.route.js";

dotenv.config();

/**
 * Express application instance
 * @type {Object}
 */
const app = express();

/**
 * Server port - uses environment variable or defaults to 5000
 * @type {number}
 */
const PORT = process.env.PORT || 5000;

const __dirname = path.resolve();

// Middleware
app.use(express.json()); // allows us to accept JSON data in the req.body

// Routes
/**
 * @route /api/products
 * @desc Product related routes
 */
app.use("/api/products", productRoutes);

/**
 * @route /api/users
 * @desc User related routes
 */
app.use("/api/users", userRoutes);

// Serve static assets in production
if (process.env.NODE_ENV === "production") {
  app.use(express.static(path.join(__dirname, "/frontend/dist")));
  app.get("*", (req, res) => {
    res.sendFile(path.resolve(__dirname, "frontend", "dist", "index.html"));
  });
}

/**
 * Start the server and connect to database
 */
app.listen(PORT, () => {
  connectDB();
  console.log("Server started at http://localhost:" + PORT);
});