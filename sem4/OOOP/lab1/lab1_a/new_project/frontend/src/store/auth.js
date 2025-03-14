import { create } from "zustand";
import { persist } from "zustand/middleware";

export const useAuthStore = create(
  persist(
    (set, get) => ({
      user: null,
      token: null,
      isAuthenticated: false,
      isAdmin: false,
      loading: false,
      
      register: async (userData) => {
        set({ loading: true });
        try {
          const res = await fetch("/api/users/register", {
            method: "POST",
            headers: {
              "Content-Type": "application/json",
            },
            body: JSON.stringify(userData),
          });
          
          const data = await res.json();
          
          if (data.success) {
            set({ 
              user: data.user,
              token: data.token,
              isAuthenticated: true,
              isAdmin: data.user.role === "admin",
              loading: false
            });
            return { success: true };
          } else {
            set({ loading: false });
            return { success: false, message: data.message };
          }
        } catch (error) {
          set({ loading: false });
          return { success: false, message: error.message };
        }
      },
      
      login: async (credentials) => {
        set({ loading: true });
        try {
          const res = await fetch("/api/users/login", {
            method: "POST",
            headers: {
              "Content-Type": "application/json",
            },
            body: JSON.stringify(credentials),
          });
          
          const data = await res.json();
          
          if (data.success) {
            set({ 
              user: data.user,
              token: data.token,
              isAuthenticated: true,
              isAdmin: data.user.role === "admin",
              loading: false
            });
            return { success: true };
          } else {
            set({ loading: false });
            return { success: false, message: data.message };
          }
        } catch (error) {
          set({ loading: false });
          return { success: false, message: error.message };
        }
      },
      
      logout: () => {
        set({ 
          user: null,
          token: null,
          isAuthenticated: false,
          isAdmin: false
        });

        window.location.href = '/';
      },
      
      getMe: async () => {
        const token = get().token;
        
        if (!token) return;
        
        set({ loading: true });
        try {
          const res = await fetch("/api/users/me", {
            headers: {
              Authorization: `Bearer ${token}`,
            },
          });
          
          const data = await res.json();
          
          if (data.success) {
            set({ 
              user: data.data,
              isAuthenticated: true,
              isAdmin: data.data.role === "admin",
              loading: false
            });
          } else {
            set({ loading: false });
          }
        } catch (error) {
          console.error("Error fetching user data:", error);
          set({ loading: false });
        }
      },
    }),
    {
      name: "auth-storage",
      partialize: (state) => ({ token: state.token, user: state.user }),
    }
  )
);