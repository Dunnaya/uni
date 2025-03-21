/**
 * Product model definition
 * @module models/product
 */

import mongoose from "mongoose";

/**
 * Product schema for MongoDB
 * @typedef {Object} ProductSchema
 * @property {string} name - Product name
 * @property {number} price - Product price
 * @property {string} image - Product image URL
 * @property {string} [description] - Optional product description
 * @property {Date} createdAt - Date when product was created
 * @property {Date} updatedAt - Date when product was last updated
 */
const productSchema = new mongoose.Schema(
	{
		name: {
			type: String,
			required: true,
		},
		price: {
			type: Number,
			required: true,
		},
		image: {
			type: String,
			required: true,
		},
		description: {
			type: String,
			required: false,
		}
	},
	{
		timestamps: true, // createdAt, updatedAt
	}
);

const Product = mongoose.model("Product", productSchema);

export default Product;