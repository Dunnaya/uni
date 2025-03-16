import { create } from "zustand";
import { persist } from "zustand/middleware";

export const useCartStore = create(
  persist(
    (set, get) => ({
      items: [],
      totalItems: 0,
      totalPrice: 0,
      
      addToCart: (product, quantity = 1) => {
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
      
      removeFromCart: (productId) => {
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
      
      updateQuantity: (productId, quantity) => {
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
      
      clearCart: () => {
        set({ items: [], totalItems: 0, totalPrice: 0 });
      },
    }),
    {
      name: "cart-storage", // localStorage key
    }
  )
);