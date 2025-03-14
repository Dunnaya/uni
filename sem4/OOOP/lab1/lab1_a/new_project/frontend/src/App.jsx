import { Box, useColorModeValue } from "@chakra-ui/react";
import { Route, Routes } from "react-router-dom";
import { useEffect } from "react";

import CreatePage from "./pages/CreatePage";
import HomePage from "./pages/HomePage";
import LoginPage from "./pages/LoginPage";
import RegisterPage from "./pages/RegisterPage";
import UnauthorizedPage from "./pages/UnauthorizedPage";
import ProductPage from "./pages/ProductPage";
import Navbar from "./components/Navbar";
import ProtectedRoute from "./components/ProtectedRoute";
import { useAuthStore } from "./store/auth";

function App() {
  const { getMe } = useAuthStore();
  
  // when loading the application, we check if there is an active session
  useEffect(() => {
    getMe();
  }, [getMe]);
  
  return (
    <Box minH={"100vh"} bg={useColorModeValue("#FFEDFA", "rgb(75, 9, 57)")}>
      <Navbar />
      <Routes>
        <Route path='/' element={<HomePage />} />
        <Route path='/product/:productId' element={<ProductPage />} /> {/* New route */}
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