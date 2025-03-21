import { Navigate, useLocation } from "react-router-dom";
import { useAuthStore } from "../store/auth"; // Changed from store/user to store/auth

/**
 * @file Protected Route component for route access control
 * @module components/ProtectedRoute
 */

/**
 * ProtectedRoute component to restrict access to authenticated users or admin users.
 * Redirects to login page if user is not authenticated or to unauthorized page if
 * user doesn't have admin rights when required.
 * 
 * @component
 * @param {Object} props - Component props
 * @param {React.ReactNode} props.children - Child components to render if access is allowed
 * @param {boolean} [props.adminOnly=false] - Whether the route requires admin privileges
 * @returns {JSX.Element} - Either the children components or a redirect
 */

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