import { create } from "zustand";
import { persist } from "zustand/middleware";
import { useAuthStore } from "./auth";

export const useCartStore = create(
  persist(
    (set, get) => ({
      items: [],
      totalItems: 0,
      totalPrice: 0,
      userId: null,
      
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
      partialize: (state) => ({
        items: state.items, 
        totalItems: state.totalItems, 
        totalPrice: state.totalPrice,
        userId: state.userId
      }),
    }
  )
);