/**
 * Product routes
 * @module routes/product
 */

import express from "express";
import {
  createProduct,
  deleteProduct,
  getProducts,
  updateProduct,
} from "../controllers/product.controller.js";
import { protect, authorize } from "../middleware/auth.middleware.js";

const router = express.Router();

/**
 * @route GET /api/products
 * @desc Get all products
 * @access Public
 */
router.get("/", getProducts);

/**
 * @route POST /api/products
 * @desc Create a new product
 * @access Private/Admin
 */
router.post("/", protect, authorize("admin"), createProduct);

/**
 * @route PUT /api/products/:id
 * @desc Update a product
 * @access Private/Admin
 */
router.put("/:id", protect, authorize("admin"), updateProduct);

/**
 * @route DELETE /api/products/:id
 * @desc Delete a product
 * @access Private/Admin
 */
router.delete("/:id", protect, authorize("admin"), deleteProduct);

export default router;