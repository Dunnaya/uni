/**
 * Product controller functions
 * @module controllers/product
 */

import mongoose from "mongoose";
import Product from "../models/product.model.js";

/**
 * Get all products
 * @async
 * @function getProducts
 * @param {Object} req - Express request object
 * @param {Object} res - Express response object
 * @returns {Object} JSON with success status and array of products
 */
export const getProducts = async (req, res) => {
	try {
		const products = await Product.find({});
		res.status(200).json({ success: true, data: products });
	} catch (error) {
		console.log("error in fetching products:", error.message);
		res.status(500).json({ success: false, message: "Server Error" });
	}
};

/**
 * Create a new product
 * @async
 * @function createProduct
 * @param {Object} req - Express request object
 * @param {Object} req.body - Request body containing product data
 * @param {string} req.body.name - Product name
 * @param {number} req.body.price - Product price
 * @param {string} req.body.image - Product image URL
 * @param {string} [req.body.description] - Optional product description
 * @param {Object} res - Express response object
 * @returns {Object} JSON with success status and created product data
 */
export const createProduct = async (req, res) => {
	const product = req.body; // user will send this data

	if (!product.name || !product.price || !product.image) {
		return res.status(400).json({ success: false, message: "Please provide all fields" });
	}

	const newProduct = new Product(product);

	try {
		await newProduct.save();
		res.status(201).json({ success: true, data: newProduct });
	} catch (error) {
		console.error("Error in Create product:", error.message);
		res.status(500).json({ success: false, message: "Server Error" });
	}
};

/**
 * Update a product by ID
 * @async
 * @function updateProduct
 * @param {Object} req - Express request object
 * @param {Object} req.params - URL parameters
 * @param {string} req.params.id - Product ID to update
 * @param {Object} req.body - Request body with updated product data
 * @param {Object} res - Express response object
 * @returns {Object} JSON with success status and updated product data
 */
export const updateProduct = async (req, res) => {
	const { id } = req.params;

	const product = req.body;

	if (!mongoose.Types.ObjectId.isValid(id)) {
		return res.status(404).json({ success: false, message: "Invalid Product Id" });
	}

	try {
		const updatedProduct = await Product.findByIdAndUpdate(id, product, { new: true });
		res.status(200).json({ success: true, data: updatedProduct });
	} catch (error) {
		res.status(500).json({ success: false, message: "Server Error" });
	}
};

/**
 * Delete a product by ID
 * @async
 * @function deleteProduct
 * @param {Object} req - Express request object
 * @param {Object} req.params - URL parameters
 * @param {string} req.params.id - Product ID to delete
 * @param {Object} res - Express response object
 * @returns {Object} JSON with success status and confirmation message
 */
export const deleteProduct = async (req, res) => {
	const { id } = req.params;

	if (!mongoose.Types.ObjectId.isValid(id)) {
		return res.status(404).json({ success: false, message: "Invalid Product Id" });
	}

	try {
		await Product.findByIdAndDelete(id);
		res.status(200).json({ success: true, message: "Product deleted" });
	} catch (error) {
		console.log("error in deleting product:", error.message);
		res.status(500).json({ success: false, message: "Server Error" });
	}
};