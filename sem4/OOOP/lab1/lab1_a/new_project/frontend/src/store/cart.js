/**
 * @file Shopping cart store using Zustand
 * @description Manages shopping cart state including items, quantities, and user-specific carts
 */

import { create } from "zustand";
import { persist } from "zustand/middleware";
import { useAuthStore } from "./auth";

/**
 * Cart store with persistent storage
 * @typedef {Object} CartState
 * @property {Array} items - Cart items
 * @property {number} totalItems - Total number of items in cart
 * @property {number} totalPrice - Total price of all items in cart
 * @property {string|null} userId - ID of the user who owns the cart
 * @property {Function} initializeCart - Initialize cart for current user
 * @property {Function} addToCart - Add item to cart
 * @property {Function} removeFromCart - Remove item from cart
 * @property {Function} updateQuantity - Update item quantity
 * @property {Function} clearCart - Clear all items from cart
 */

/**
 * Create cart store with persistence
 */
export const useCartStore = create(
  persist(
    (set, get) => ({
      items: [],
      totalItems: 0,
      totalPrice: 0,
      userId: null,
      
      /**
       * Initialize cart for current user
       * Ensures cart belongs to the currently logged-in user
       */
      initializeCart: () => {
        const authState = useAuthStore.getState();
        const currentUserId = authState.user?._id;
        
        // if user is logged in and userId doesn't match current user
        if (authState.isAuthenticated && currentUserId !== get().userId) {
          // reset cart for new user
          set({ 
            items: [], 
            totalItems: 0, 
            totalPrice: 0, 
            userId: currentUserId 
          });
        } else if (!authState.isAuthenticated) {
          // clear userId if not authenticated
          set(state => ({ ...state, userId: null }));
        }
      },
      
      /**
       * Add product to cart
       * @param {Object} product - Product to add
       * @param {string} product._id - Product ID
       * @param {string} product.name - Product name
       * @param {number} product.price - Product price
       * @param {number} [quantity=1] - Quantity to add
       */
      addToCart: (product, quantity = 1) => {
        // make sure cart is initialized with correct user
        get().initializeCart();
        
        set((state) => {
          const existingItem = state.items.find(item => item._id === product._id);
          
          let updatedItems;
          if (existingItem) {
            // update quantity if item exists
            updatedItems = state.items.map(item => 
              item._id === product._id 
                ? { ...item, quantity: item.quantity + quantity } 
                : item
            );
          } else {
            // add new item
            updatedItems = [...state.items, { ...product, quantity }];
          }
          
          // calc total items and price
          const totalItems = updatedItems.reduce((sum, item) => sum + item.quantity, 0);
          const totalPrice = updatedItems.reduce(
            (sum, item) => sum + item.price * item.quantity, 
            0
          );
          
          return { items: updatedItems, totalItems, totalPrice };
        });
      },
      
      /**
       * Remove product from cart
       * @param {string} productId - ID of product to remove
       */
      removeFromCart: (productId) => {
        // make sure cart is initialized with correct user
        get().initializeCart();
        
        set((state) => {
          const updatedItems = state.items.filter(item => item._id !== productId);
          
          // calculate total items and price
          const totalItems = updatedItems.reduce((sum, item) => sum + item.quantity, 0);
          const totalPrice = updatedItems.reduce(
            (sum, item) => sum + item.price * item.quantity, 
            0
          );
          
          return { items: updatedItems, totalItems, totalPrice };
        });
      },
      
      /**
       * Update product quantity in cart
       * @param {string} productId - ID of product to update
       * @param {number} quantity - New quantity (minimum 1)
       */
      updateQuantity: (productId, quantity) => {
        // make sure cart is initialized with correct user
        get().initializeCart();
        
        set((state) => {
          // don't allow quantities less than 1
          if (quantity < 1) return state;
          
          const updatedItems = state.items.map(item => 
            item._id === productId ? { ...item, quantity } : item
          );
          
          // calculate total items and price
          const totalItems = updatedItems.reduce((sum, item) => sum + item.quantity, 0);
          const totalPrice = updatedItems.reduce(
            (sum, item) => sum + item.price * item.quantity, 
            0
          );
          
          return { items: updatedItems, totalItems, totalPrice };
        });
      },
      
      /**
       * Clear all items from cart
       * Maintains the userId when clearing cart
       */
      clearCart: () => {
        const authState = useAuthStore.getState();
        const currentUserId = authState.user?._id;
        
        set({ 
          items: [], 
          totalItems: 0, 
          totalPrice: 0,
          userId: currentUserId  // maintain the userId when clearing cart
        });
      },
    }),
    {
      name: "cart-storage", // localStorage key
      /**
       * Specify which parts of state to persist in storage
       * @param {CartState} state - Current state
       * @returns {Object} Partial state to persist
       */
      partialize: (state) => ({
        items: state.items, 
        totalItems: state.totalItems, 
        totalPrice: state.totalPrice,
        userId: state.userId
      }),
    }
  )
);