import numpy as np

def setupLogging(name):
    def log(msg):
        print(f"[{name}] {msg}")
    return log

def isPsevdoInversed(A, A_pinv) -> bool:
    """Checking Moore–Penrose conditions"""
    if not np.allclose(np.dot(np.dot(A, A_pinv), A), A):
        print('Wrong! A * A+ * A != A')
        return False
    if not np.allclose(np.dot(np.dot(A_pinv, A), A_pinv), A_pinv):
        print('Wrong! A+ * A * A+ != A+')
        return False
    if not np.allclose(np.dot(A, A_pinv), np.dot(A, A_pinv).T):
        print('Wrong! A * A+ is not symmetric')
        return False
    if not np.allclose(np.dot(A_pinv, A), np.dot(A_pinv, A).T):
        print('Wrong! A+ * A is not symmetric')
        return False
    print('Matrix is pseudoinverse (Moore–Penrose).')
    return True

def pseudoInverseMatrix_Greville(A, eps = 1e-12, delta = None):
    log = setupLogging('Greville')
    A = np.array(A, dtype=float)
    m, n = A.shape

    a1 = A[0, :].reshape(-1, 1)  
    denom = np.dot(a1.T, a1)
    if np.abs(denom) < eps:
        A_plus = np.zeros((n, 1))
    else:
        A_plus = np.dot(a1, 1 / denom)  # (a1) / (a1^T a1)
    current_A = np.array([A[0]])

    log("Step 0: first row processed.")
    log(f"A_plus shape: {A_plus.shape}\n")
    
    for i in range(1, m):
        a = A[i, :].reshape(-1, 1)    
        Z = np.identity(current_A.shape[1]) - np.dot(A_plus, current_A)
        quad_form = np.dot(a.T, np.dot(Z, a))[0, 0] # a^TZa

        if quad_form > eps:
            num1 = np.dot(Z, np.dot(a, np.dot(a.T, A_plus)))
            left = A_plus - num1 / quad_form
            right = np.dot(Z, a) / quad_form
            A_plus = np.hstack((left, right))
        else:
            R = np.dot(A_plus, A_plus.T)
            denom = 1 + np.dot(a.T, np.dot(R, a))[0, 0]
            num2 = np.dot(R, np.dot(a, np.dot(a.T, A_plus)))
            left = A_plus - num2 / denom
            right = np.dot(R, a) / denom
            A_plus = np.hstack((left, right))

        current_A = np.vstack([current_A, A[i]])
        log(f"Step {i}: A_plus shape: {A_plus.shape}")

    log(f"Finished. Final shape: {A_plus.shape}\n")
    return A_plus, m - 1

def pseudoInverseMatrix_MoorePenrose(A, eps = 1e-6, delta = 1000):
    log = setupLogging('MoorePenrose')
    m, n = A.shape
    
    A0 = A.T @ np.linalg.inv(A @ A.T + delta**2 * np.identity(m))
    log(f'Initial form A0: {A0.shape}')
    
    delta = delta / 2

    iterations = 0
    while True:
        A_plus = A.T @ np.linalg.inv(A @ A.T + delta**2 * np.identity(m))

        if np.linalg.norm(A0 - A_plus, ord = 2) < eps:
            log(f'Converged after {iterations} iterations\n')
            return A_plus, iterations
        
        delta = delta / 2
        A0 = A_plus
        iterations += 1
        
        # protection against infinite loop
        if iterations > 1000:
            log(f'Warning: Maximum iterations reached (1000)\n')
            return A_plus, iterations

# testing
if __name__ == "__main__":
    A = np.array([
        [1, 2, 3],
        [4, 5, 6],
        [7, 8, 9],
    ], dtype=float)

    print("Initial matrix A:")
    print(A, "\n")

    A_plus_g, iters_g = pseudoInverseMatrix_Greville(A)

    print("\nPseudoinverse matrix (Greville):")
    print(A_plus_g, "\n")

    print("Checking Moore–Penrose conditions:")
    isPsevdoInversed(A, A_plus_g)

    print("\nComparison with numpy.linalg.pinv:")
    print("Difference:", np.max(np.abs(A_plus_g - np.linalg.pinv(A))))
    print("Are they equal?", np.allclose(A_plus_g, np.linalg.pinv(A)))

    A_pinv, iters = pseudoInverseMatrix_MoorePenrose(A)

    print("\nPseudoinverse matrix (Moore–Penrose):")
    print(A_pinv)
    print(f"\nNumber of iterations: {iters}")

    print("\nChecking with NumPy:")
    print("Max difference:", np.max(np.abs(A_pinv - np.linalg.pinv(A))))
    print("Are they equal?", np.allclose(A_pinv, np.linalg.pinv(A)))