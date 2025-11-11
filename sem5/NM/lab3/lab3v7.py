import numpy as np

def power_method(A, eps = 1e-4, max_iter = 50):
    n = A.shape[0]
    x = np.ones(n)       
    lam_old = 0                 
    m = n - 1

    print("\t\nPower Method")
    print("Initial vector x0:", x)
    print("Eps:", eps)
    print("Maximum iterations:", max_iter)\
    
    for k in range(max_iter):
        print(f"\nIteration {k + 1}:")
        # multiply A by the current vector
        x_new = A @ x
        print("A * x =", np.round(x_new, 3))

        # estimate new eigenvalue
        lam_new = x_new[m] / x[m]
        print(f"λ_new = x_new[{m}] / x[{m}] = {lam_new:.3f}")

        # normalize the resulting vector
        x_new_norm = np.linalg.norm(x_new)
        x = x_new / x_new_norm
        x_display = np.where(np.abs(x) < 1e-6, 0, np.round(x, 3))
        print(f"||x_new|| = {x_new_norm:.3f}")
        print(f"Normalized vector e{k + 1} =", x_display)

        # check convergence
        if np.abs(lam_new - lam_old) <= eps:
            print("\n")
            print(f"Converged after {k + 1} iterations.")
            print(f"Dominant eigenvalue λ ≈ {lam_new:.4f}")
            return lam_new
        
        lam_old = lam_new

    print("Method did not converge within the iteration limit.")
    print(f"Last estimated λ = {lam_new:.4f}")
    print("\n")
    return lam_new

def min_eigenvalue(A):
    n = A.shape[0]
    # check conditiond to use the power method 
    # A = A^T >0
    if not np.allclose(A, A.T):
        print("Matrix A isn't symmetrical. Method can't be applied.")
        return
    
    matrix_minor = np.zeros(n)
    print("Matrix minors:")
    for i in range(1, n + 1):
        minor = np.linalg.det(A[:i, :i])
        matrix_minor[i - 1] = round(minor)
        print (f" |A{i}| = {matrix_minor[i-1]}") 
        if minor <= 0:
            print(f"Matrix is not positive definite (|A{i}| = {round(minor)})")
            return None

    print("\nConditions for the power method are met.")
    
    normA = np.max(np.sum(np.abs(A), axis=1)) 
    print("\nMax ||A|| =",normA)
    print()

    # B = ||A||E - A
    B = normA * np.eye(len(A)) - A     
    print("B = ||A||E - A :\n", B)
    
    # find greatest eigenvalue of B matrix by power method        
    lam_max_B = power_method(B) 
    print("\nmax_lam(B) = ",lam_max_B)

    lam_min_A = normA - lam_max_B
    print("\nmin_lam(A) = ||A|| - max_lam_B")  
    print(f"\nmin_lam(A) = {normA} - {lam_max_B}")
    print(f"\nmin_lam(A) = {lam_min_A}")           
    return lam_min_A

def jacobi_method(A, eps = 1e-4, max_iter = 50):
    np.set_printoptions(precision=6, suppress=True)

    if not np.allclose(A, A.T):
        print("Matrix A isn't symmetrical. Jacobi method can't be applied.")
        return None, None

    n = A.shape[0]
    V = np.eye(n)  # for eigenvectors

    print("\t\nJacobi Rotation Method")
    print("Initial matrix A:")
    print(np.round(A, 6))
    print("Eps:", eps)
    print("Maximum iterations:", max_iter)

    for k in range(1, max_iter + 1):
        p, q = 0, 1
        max_val = 0.0
        for i in range(n):
            for j in range(i + 1, n):
                if abs(A[i, j]) > abs(max_val):
                    max_val = A[i, j]
                    p, q = i, j

        print(f"\nIteration {k}:")
        print(f"  Pivot indices (p, q) = ({p}, {q})")
        print(f"  a[p,q] = {A[p,q]:.6f}")

        # stop check
        off_norm = np.sqrt(np.sum(A**2) - np.sum(np.diag(A)**2))
        print(f"  Off-diagonal norm = {np.round(off_norm,6)}")
        if off_norm < eps:
            print("\nConvergence criterion satisfied (off-diagonal norm < eps).")
            break

        # compute rotation angle φ_k (according to theory)
        a_pp = A[p, p]
        a_qq = A[q, q]
        a_pq = A[p, q]

        if a_pp == a_qq:
            phi_k = np.pi / 4
        else:
            phi_k = 0.5 * np.arctan((2 * a_pq) / (a_pp - a_qq))

        c = np.cos(phi_k)
        s = np.sin(phi_k)

        print(f"  φ_k = 0.5 * arctan(2 * a[{p},{q}] / (a[{p},{p}] - a[{q},{q}]))")
        print(f"  φ_k = {phi_k:.6f}")
        print(f"  cos(φ_k) = {c:.6f}")
        print(f"  sin(φ_k) = {s:.6f}")

        # build full rotation matrix U_k
        U = np.eye(n)
        U[p, p] = c
        U[q, q] = c
        U[p, q] = -s
        U[q, p] = s

        print("\n  Rotation matrix U_k:")
        print(np.round(U, 6))

        # apply transformation A_{k+1} = U^T * A * U
        A = U.T @ A @ U

        # accumulate eigenvectors: V = V * U
        V = V @ U

        # print updated matrix A
        print("\n  Matrix A after rotation:")
        print(np.round(A, 4))

    # after iterations, diagonal of A are approximated eigenvalues
    eigvals = np.diag(A).copy()
    eigvecs = V.copy()

    print("\n")
    print(f"Finished after {k} iterations.")
    print("Approximated eigenvalues:")
    print(np.round(eigvals, 6))
    print("\nCorresponding eigenvectors (columns):")
    print(np.round(eigvecs, 4))
    print("\n")

    return eigvals, eigvecs

if __name__ == "__main__":
    A = np.array([[5,0,2,1],
                  [0,4,0,1],
                  [2,0,2,0],
                  [1,1,0,3]])
    
    min_eigenvalue(A)
    jacobi_method(A)