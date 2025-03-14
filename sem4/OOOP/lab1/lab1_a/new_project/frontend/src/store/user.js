import { create } from "zustand";
import { persist } from "zustand/middleware";

export const useAuthStore = create(
  persist(
    (set) => ({
      user: null,
      token: null,
      isAuthenticated: false,
      loading: false,
      error: null,
      
      // register
      register: async (userData) => {
        set({ loading: true, error: null });
        try {
          const res = await fetch("/api/users/register", {
            method: "POST",
            headers: {
              "Content-Type": "application/json",
            },
            body: JSON.stringify(userData),
          });
          
          const data = await res.json();
          
          if (!data.success) {
            set({ loading: false, error: data.message });
            return { success: false, message: data.message };
          }
          
          set({ 
            user: data.user, 
            token: data.token, 
            isAuthenticated: true, 
            loading: false 
          });
          
          return { success: true };
        } catch (error) {
          set({ loading: false, error: error.message });
          return { success: false, message: error.message };
        }
      },
      
      // log in
      login: async (userData) => {
        set({ loading: true, error: null });
        try {
          const res = await fetch("/api/users/login", {
            method: "POST",
            headers: {
              "Content-Type": "application/json",
            },
            body: JSON.stringify(userData),
          });
          
          const data = await res.json();
          
          if (!data.success) {
            set({ loading: false, error: data.message });
            return { success: false, message: data.message };
          }
          
          set({ 
            user: data.user, 
            token: data.token, 
            isAuthenticated: true, 
            loading: false 
          });
          
          return { success: true };
        } catch (error) {
          set({ loading: false, error: error.message });
          return { success: false, message: error.message };
        }
      },
      
      logout: () => {
        set({ user: null, token: null, isAuthenticated: false });
      },
      
      checkUser: async () => {
        const state = useAuthStore.getState();
        if (!state.token) return;
        
        set({ loading: true });
        try {
          const res = await fetch("/api/users/me", {
            headers: {
              Authorization: `Bearer ${state.token}`,
            },
          });
          
          const data = await res.json();
          
          if (!data.success) {
            set({ loading: false, user: null, token: null, isAuthenticated: false });
            return;
          }
          
          set({ loading: false, user: data.data });
        } catch (error) {
          set({ loading: false, user: null, token: null, isAuthenticated: false });
        }
      },
    }),
    {
      name: "auth-storage", // name for localStorage
      partialize: (state) => ({ user: state.user, token: state.token, isAuthenticated: state.isAuthenticated }),
    }
  )
);