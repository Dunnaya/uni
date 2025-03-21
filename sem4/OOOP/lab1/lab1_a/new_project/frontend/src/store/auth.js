/**
 * @file Authentication store using Zustand
 * @description Manages user authentication state including login, registration, and session persistence
 */

import { create } from "zustand";
import { persist } from "zustand/middleware";

/**
 * Authentication store with persistent storage
 * @typedef {Object} AuthState
 * @property {Object|null} user - Current user data
 * @property {string|null} token - Authentication token
 * @property {boolean} isAuthenticated - Whether user is authenticated
 * @property {boolean} isAdmin - Whether user has admin privileges
 * @property {boolean} loading - Loading state for async operations
 * @property {Function} register - Register a new user
 * @property {Function} login - Log in an existing user
 * @property {Function} logout - Log out the current user
 * @property {Function} getMe - Fetch current user data
 */

/**
 * Create auth store with persistence
 */
export const useAuthStore = create(
  persist(
    (set, get) => ({
      user: null,
      token: null,
      isAuthenticated: false,
      isAdmin: false,
      loading: false,
      
      /**
       * Register a new user
       * @async
       * @param {Object} userData - User registration data
       * @param {string} userData.email - User's email
       * @param {string} userData.password - User's password
       * @param {string} userData.name - User's name
       * @returns {Promise<Object>} Result object with success status and optional message
       */
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
      
      /**
       * Log in an existing user
       * @async
       * @param {Object} credentials - User login credentials
       * @param {string} credentials.email - User's email
       * @param {string} credentials.password - User's password
       * @returns {Promise<Object>} Result object with success status and optional message
       */
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
      
      /**
       * Log out the current user
       * Clears user data and redirects to home page
       */
      logout: () => {
        set({ 
          user: null,
          token: null,
          isAuthenticated: false,
          isAdmin: false
        });

        window.location.href = '/';
      },
      
      /**
       * Fetch current user data from API
       * @async
       * @returns {Promise<void>}
       */
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
      /**
       * Specify which parts of state to persist in storage
       * @param {AuthState} state - Current state
       * @returns {Object} Partial state to persist
       */
      partialize: (state) => ({ token: state.token, user: state.user }),
    }
  )
);