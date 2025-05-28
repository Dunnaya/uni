// controllers/product.controller.js
import { ProductService } from "../services/ProductService.js";
import { ProductValidationStrategy } from "../validation/ValidationStrategy.js";
import { ResponseDirector } from "../utils/responseBuilder.js";
import { MongooseAdapter } from "../adapters/DatabaseAdapter.js";
import Product from "../models/product.model.js";
import User from "../models/user.model.js";

const databaseAdapter = new MongooseAdapter({ Product, User });
const validationStrategy = new ProductValidationStrategy();
const responseDirector = new ResponseDirector();
const productService = new ProductService(databaseAdapter, validationStrategy, responseDirector);

export const getProducts = async (req, res) => {
  try {
    const result = await productService.getAllProducts();
    const statusCode = result.success ? 200 : 500;
    res.status(statusCode).json(result);
  } catch (error) {
    const errorResponse = responseDirector.buildErrorResponse(
      error.message,
      'Failed to fetch products'
    );
    res.status(500).json(errorResponse);
  }
};

export const createProduct = async (req, res) => {
  try {
    const userId = req.user?.id;
    const result = await productService.createProduct(req.body, userId);
    const statusCode = result.success ? 201 : 400;
    res.status(statusCode).json(result);
  } catch (error) {
    const errorResponse = responseDirector.buildErrorResponse(
      error.message,
      'Failed to create product'
    );
    res.status(500).json(errorResponse);
  }
};

export const updateProduct = async (req, res) => {
  try {
    const { id } = req.params;
    const userId = req.user?.id;
    const result = await productService.updateProduct(id, req.body, userId);
    const statusCode = result.success ? 200 : (result.message.includes('not found') ? 404 : 400);
    res.status(statusCode).json(result);
  } catch (error) {
    const errorResponse = responseDirector.buildErrorResponse(
      error.message,
      'Failed to update product'
    );
    res.status(500).json(errorResponse);
  }
};

export const deleteProduct = async (req, res) => {
  try {
    const { id } = req.params;
    const result = await productService.deleteProduct(id);
    const statusCode = result.success ? 200 : (result.message.includes('not found') ? 404 : 400);
    res.status(statusCode).json(result);
  } catch (error) {
    const errorResponse = responseDirector.buildErrorResponse(
      error.message,
      'Failed to delete product'
    );
    res.status(500).json(errorResponse);
  }
};