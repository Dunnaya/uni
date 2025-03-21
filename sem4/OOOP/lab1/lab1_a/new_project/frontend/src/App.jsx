import { Box, useColorModeValue } from "@chakra-ui/react";
import { Route, Routes } from "react-router-dom";
import { useEffect } from "react";

import CreatePage from "./pages/CreatePage";
import HomePage from "./pages/HomePage";
import LoginPage from "./pages/LoginPage";
import RegisterPage from "./pages/RegisterPage";
import UnauthorizedPage from "./pages/UnauthorizedPage";
import ProductPage from "./pages/ProductPage";
import CartPage from "./pages/CartPage";
import Navbar from "./components/Navbar";
import ProtectedRoute from "./components/ProtectedRoute";
import { useAuthStore } from "./store/auth";
import { useCartStore } from "./store/cart";

/**
 * @file Main application component with routing
 * @module App
 */

/**
 * App component sets up the application routes and layout.
 * Initializes authentication and cart state on application load.
 * 
 * @component
 * @returns {JSX.Element} - Rendered App component with routes
 */

function App() {
  const { getMe } = useAuthStore();
  const { initializeCart } = useCartStore();
  
  // when loading the application, we check if there is an active session
  useEffect(() => {
    getMe();
  }, [getMe]);
  
  // initialize cart when auth state changes
  useEffect(() => {
    initializeCart();
  }, [initializeCart, useAuthStore.getState().user]);
  
  return (
    <Box minH={"100vh"} bg={useColorModeValue("#FFEDFA", "rgb(75, 9, 57)")}>
      <Navbar />
      <Routes>
        <Route path='/' element={<HomePage />} />
        <Route path='/product/:productId' element={<ProductPage />} />
        <Route path='/cart' element={
          <ProtectedRoute>
            <CartPage />
          </ProtectedRoute>
        } />
        <Route path='/create' element={
          <ProtectedRoute adminOnly={true}>
            <CreatePage />
          </ProtectedRoute>
        } />
        <Route path='/login' element={<LoginPage />} />
        <Route path='/register' element={<RegisterPage />} />
        <Route path='/unauthorized' element={<UnauthorizedPage />} />
      </Routes>
    </Box>
  );
}

export default App;