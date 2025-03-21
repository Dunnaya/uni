/**
 * @file User authentication store using Zustand
 * @description Manages user authentication state including registration, login, and session checking
 */

import { create } from "zustand";
import { persist } from "zustand/middleware";

/**
 * User authentication store with persistent storage
 * @typedef {Object} UserAuthState
 * @property {Object|null} user - Current user data
 * @property {string|null} token - Authentication token
 * @property {boolean} isAuthenticated - Whether user is authenticated
 * @property {boolean} loading - Loading state for async operations
 * @property {string|null} error - Error message if any
 * @property {Function} register - Register a new user
 * @property {Function} login - Log in an existing user
 * @property {Function} logout - Log out the current user
 * @property {Function} checkUser - Verify current user session
 */

/**
 * Create user authentication store with persistence
 */
export const useAuthStore = create(
  persist(
    (set) => ({
      user: null,
      token: null,
      isAuthenticated: false,
      loading: false,
      error: null,
      
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
      
      /**
       * Log in an existing user
       * @async
       * @param {Object} userData - User login credentials
       * @param {string} userData.email - User's email
       * @param {string} userData.password - User's password
       * @returns {Promise<Object>} Result object with success status and optional message
       */
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
      
      /**
       * Log out the current user
       * Clears all authentication state
       */
      logout: () => {
        set({ user: null, token: null, isAuthenticated: false });
      },
      
      /**
       * Verify current user session
       * @async
       * Fetches current user data from server to validate session
       */
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
      /**
       * Specify which parts of state to persist in storage
       * @param {UserAuthState} state - Current state
       * @returns {Object} Partial state to persist
       */
      partialize: (state) => ({ user: state.user, token: state.token, isAuthenticated: state.isAuthenticated }),
    }
  )
);