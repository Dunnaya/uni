import { Navigate, useLocation } from "react-router-dom";
import { useAuthStore } from "../store/auth"; // Changed from store/user to store/auth

// updated to handle adminOnly parameter correctly
const ProtectedRoute = ({ children, adminOnly = false }) => {
  const { isAuthenticated, isAdmin } = useAuthStore();
  const location = useLocation();

  // redirect to login if not authenticated
  if (!isAuthenticated) {
    return <Navigate to="/login" state={{ from: location }} replace />;
  }

  // if admin only and user is not admin, redirect to unauthorized
  if (adminOnly && !isAdmin) {
    return <Navigate to="/unauthorized" replace />;
  }

  // if all checks pass, display the children
  return children;
};

export default ProtectedRoute;