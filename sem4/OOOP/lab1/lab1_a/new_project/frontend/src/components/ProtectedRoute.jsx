import { Navigate, useLocation } from "react-router-dom";
import { useAuthStore } from "../store/user";

// component for protecting routes that require authentication
const ProtectedRoute = ({ children, allowedRoles = ["user", "admin"] }) => {
  const { isAuthenticated, user } = useAuthStore();
  const location = useLocation();

  // redirect on login page
  if (!isAuthenticated) {
    return <Navigate to="/login" state={{ from: location }} replace />;
  }

  // if allowed roles are specified and the user's role is not included in them,
  // redirect to the home page
  if (allowedRoles.length > 0 && !allowedRoles.includes(user.role)) {
    return <Navigate to="/" replace />;
  }

  // if all checks are passed, display the child components
  return children;
};

export default ProtectedRoute;