/**
 * @file Product store using Zustand
 * @description Manages product data and CRUD operations for products
 */

import { create } from "zustand";
import { useAuthStore } from "./auth";

/**
 * Product store
 * @typedef {Object} ProductState
 * @property {Array} products - List of products
 * @property {Function} setProducts - Set products list
 * @property {Function} createProduct - Create a new product
 * @property {Function} fetchProducts - Fetch all products
 * @property {Function} deleteProduct - Delete a product
 * @property {Function} updateProduct - Update a product
 */

/**
 * Create product store
 */
export const useProductStore = create((set) => ({
  products: [],
  
  /**
   * Set products list
   * @param {Array} products - Products array
   */
  setProducts: (products) => set({ products }),
  
  /**
   * Create a new product
   * @async
   * @param {Object} newProduct - Product to create
   * @param {string} newProduct.name - Product name
   * @param {string} newProduct.image - Product image URL
   * @param {number} newProduct.price - Product price
   * @param {string} [newProduct.description] - Product description
   * @returns {Promise<Object>} Result object with success status and message
   */
  createProduct: async (newProduct) => {
    if (!newProduct.name || !newProduct.image || !newProduct.price) {
      return { success: false, message: "Please fill in all fields." };
    }
    
    const token = useAuthStore.getState().token;
    
    if (!token) {
      return { success: false, message: "Authentication required" };
    }
    
    try {
      const res = await fetch("/api/products", {
        method: "POST",
        headers: {
          "Content-Type": "application/json",
          Authorization: `Bearer ${token}`,
        },
        body: JSON.stringify(newProduct),
      });
      
      const data = await res.json();
      
      if (!data.success) {
        return { success: false, message: data.message };
      }
      
      set((state) => ({ products: [...state.products, data.data] }));
      return { success: true, message: "Product created successfully" };
    } catch (error) {
      return { success: false, message: error.message };
    }
  },
  
  /**
   * Fetch all products
   * @async
   */
  fetchProducts: async () => {
    try {
      const res = await fetch("/api/products");
      const data = await res.json();
      if (data.success) {
        set({ products: data.data });
      }
    } catch (error) {
      console.error("Error fetching products:", error);
    }
  },
  
  /**
   * Delete a product
   * @async
   * @param {string} pid - Product ID to delete
   * @returns {Promise<Object>} Result object with success status and message
   */
  deleteProduct: async (pid) => {
    const token = useAuthStore.getState().token;
    
    if (!token) {
      return { success: false, message: "Authentication required" };
    }
    
    try {
      const res = await fetch(`/api/products/${pid}`, {
        method: "DELETE",
        headers: {
          Authorization: `Bearer ${token}`,
        },
      });
      
      const data = await res.json();
      
      if (!data.success) {
        return { success: false, message: data.message };
      }
      
      // upd UI 
      set((state) => ({ products: state.products.filter((product) => product._id !== pid) }));
      return { success: true, message: data.message };
    } catch (error) {
      return { success: false, message: error.message };
    }
  },
  
  /**
   * Update a product
   * @async
   * @param {string} pid - Product ID to update
   * @param {Object} updatedProduct - Updated product data
   * @param {string} [updatedProduct.name] - Product name
   * @param {string} [updatedProduct.image] - Product image URL
   * @param {number} [updatedProduct.price] - Product price
   * @param {string} [updatedProduct.description] - Product description
   * @returns {Promise<Object>} Result object with success status and message
   */
  updateProduct: async (pid, updatedProduct) => {
    const token = useAuthStore.getState().token;
    
    if (!token) {
      return { success: false, message: "Authentication required" };
    }
    
    try {
      const res = await fetch(`/api/products/${pid}`, {
        method: "PUT",
        headers: {
          "Content-Type": "application/json",
          Authorization: `Bearer ${token}`,
        },
        body: JSON.stringify(updatedProduct),
      });
      
      const data = await res.json();
      
      if (!data.success) {
        return { success: false, message: data.message };
      }
      
      // upd UI
      set((state) => ({
        products: state.products.map((product) => (product._id === pid ? data.data : product)),
      }));
      
      return { success: true, message: data.message };
    } catch (error) {
      return { success: false, message: error.message };
    }
  },
}));