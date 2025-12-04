import copy

def print_matrix(A, b=None, title=""):
    print(f"\n{title}")
    n = len(A)
    for i in range(n):
        row = "".join(f"{A[i][j]:8.3f}" for j in range(n))
        if b:
            print(f"[{row} | {b[i]:8.3f}]")
        else:
            print(f"[{row} ]")
    print("-" * (10 * n + 6))

def mat_mul(A, B):
    n = len(A)
    C = [[0.0] * n for _ in range(n)]
    for i in range(n):
        for j in range(n):
            for k in range(n):
                C[i][j] += A[i][k] * B[k][j]
    return C

def mat_vec_mul(A, b):
    n = len(A)
    res = [0.0] * n
    for i in range(n):
        for j in range(n):
            res[i] += A[i][j] * b[j]
    return res

def gaussian_elimination(A, b):
    n = len(A)
    A_work = copy.deepcopy(A)
    b_work = b[:]
    
    # identity matrix to be transformed into inverse components
    E = [[1.0 if i == j else 0.0 for j in range(n)] for i in range(n)]
    
    print_matrix(A_work, b_work, "Initial Matrix:")

    # forward elimination (forming P and M)
    for k in range(n):
        print(f"Step {k + 1}: Pivoting for column {k + 1}")
        
        # pivoting
        max_row = max(range(k, n), key=lambda i: abs(A_work[i][k]))
        pivot = A_work[max_row][k]
        
        if abs(pivot) < 1e-12:
            raise ValueError("Matrix is singular")

        # form Permutation Matrix Pk
        Pk = [[1.0 if i == j else 0.0 for j in range(n)] for i in range(n)]
        
        # swap rows in Pk if needed
        if max_row != k:
            print(f"Swapping rows {k + 1} and {max_row + 1}")
            Pk[k], Pk[max_row] = Pk[max_row], Pk[k]
        
        print_matrix(Pk, title=f"Permutation matrix P_{k+1}:")

        # Pk: A' = Pk * A, b' = Pk * b, E' = Pk * E
        A_work = mat_mul(Pk, A_work)
        b_work = mat_vec_mul(Pk, b_work)
        E = mat_mul(Pk, E)
        
        # curr pivot after swap
        pivot = A_work[k][k]

        # form elimination matrix Mk, m_kk = 1/pivot, m_ik = -a_ik/pivot
        Mk = [[1.0 if i == j else 0.0 for j in range(n)] for i in range(n)]
        
        Mk[k][k] = 1.0 / pivot
        for i in range(k + 1, n):
            Mk[i][k] = -A_work[i][k] / pivot
            
        print_matrix(Mk, title=f"Elimination matrix M_{k+1}:")

        # Mk: A'' = Mk * A', b'' = Mk * b, E'' = Mk * E
        A_work = mat_mul(Mk, A_work)
        b_work = mat_vec_mul(Mk, b_work)
        E = mat_mul(Mk, E)

        print_matrix(A_work, b_work, f"Matrix after step {k+1}:")
    
    # back substitution for vector x
    print("\nBack substitution:")
    x = [0.0] * n
    for i in range(n - 1, -1, -1):
        s = sum(A_work[i][j] * x[j] for j in range(i + 1, n))
        x[i] = (b_work[i] - s) # no division needed as diag is 1
        print(f"x{i + 1} = {x[i]:.6f}")

    # inverse matrix calc (A_work * X = E)
    print("\nInverse matrix calculation:")
    inv_A = [[0.0] * n for _ in range(n)]
    
    E_T = list(zip(*E)) 
    
    for col_idx in range(n):
        # b_col from transformed identity matrix
        b_col = E_T[col_idx]
        
        # back substitution for this col
        x_col = [0.0] * n
        for i in range(n - 1, -1, -1):
            s = sum(A_work[i][j] * x_col[j] for j in range(i + 1, n))
            x_col[i] = (b_col[i] - s)
        
        for i in range(n):
            inv_A[i][col_idx] = x_col[i]

    print_matrix(inv_A, b=[0]*n, title="Inverse matrix:")

    # determinant
    det = calculate_determinant(A)
    print(f"\nDeterminant of the matrix: {det:.6f}")
    
    return x, 0, inv_A

def calculate_determinant(A_in):
    A = [row[:] for row in A_in]
    n = len(A)
    det = 1.0
    for k in range(n):
        max_row = max(range(k, n), key=lambda i: abs(A[i][k]))
        if k != max_row:
            A[k], A[max_row] = A[max_row], A[k]
            det *= -1
        
        pivot = A[k][k]
        if abs(pivot) < 1e-12: return 0.0
        det *= pivot
        
        for i in range(k + 1, n):
            factor = A[i][k] / pivot
            for j in range(k, n):
                A[i][j] -= factor * A[k][j]
    return det

def tridiagonal_matrix_algorithm(A, f):
    n = len(A)
    a = [0.0] + [A[i][i - 1] for i in range(1, n)] # sub-diagonal
    c = [A[i][i] for i in range(n)] # main diagonal
    b = [A[i][i + 1] for i in range(n - 1)] + [0.0] # super-diagonal (upper)

    print("\nTridiagonal matrix algorithm (Thomas method):")
    print_matrix(A, f, "Initial tridiagonal matrix:")

    # stability check
    print(" Checking stability condition:")
    stable = all(abs(c[i]) >= abs(a[i]) + abs(b[i]) for i in range(n))
    if not stable:
        print("The stability condition of the Thomas method IS NOT met.")
    else:
        print("The stability condition of the Thomas method IS met.")
    
    # forward sweep
    print("\n Forward sweep:")
    alpha = [0.0] * n
    beta = [0.0] * n
    z = [0.0] * n

    # i = 0
    alpha[0] = -b[0] / c[0]
    beta[0] = f[0] / c[0]
    z[0] = c[0]
    print(f"i = 1: alpha1 = {-b[0]:.3f} / {c[0]:.3f} = {alpha[0]:.3f}, "
          f"beta1 = {f[0]:.3f} / {c[0]:.3f} = {beta[0]:.3f}")

    for i in range(1, n):
        z[i] = c[i] + a[i] * alpha[i - 1]
        alpha[i] = -b[i] / z[i]
        beta[i] = (f[i] - a[i] * beta[i - 1]) / z[i]
        print(f"i = {i+1}: z{i+1} = {c[i]:.3f} + ({a[i]:.3f}) * ({alpha[i-1]:.3f}) = {z[i]:.3f}")
        print(f"      alpha{i+1} = {-b[i]:.3f} / {z[i]:.3f} = {alpha[i]:.3f}")
        print(f"      beta{i+1} = ({f[i]:.3f} - {a[i]:.3f} * {beta[i-1]:.3f}) / {z[i]:.3f} = {beta[i]:.3f}")

    # back substitution
    print("\n Back substitution:")
    x = [0.0] * n
    x[-1] = beta[-1]
    print(f"x{n} = beta{n} = {x[-1]:.6f}")
    
    for i in range(n - 2, -1, -1):
        x[i] = alpha[i] * x[i + 1] + beta[i]
        print(f"x{i+1} = alpha{i+1} * x{i+2} + beta{i+1} = "
              f"{alpha[i]:.3f} * {x[i+1]:.3f} + {beta[i]:.3f} = {x[i]:.6f}")

    # final solution
    print("\n Final solution:")
    for i in range(n):
        print(f"x{i + 1} = {x[i]:.6f}")

    return x

def jacobi_method(A, b, eps=1e-3, max_iter=100):
    n = len(A)
    x = [0.0] * n
    x_new = [0.0] * n

    print("\nJacobi method:")
    print_matrix(A, b, "Initial matrix:")

    # checking convergence condition
    print(" Checking convergence condition:")
    diag_dom = True
    for i in range(n):
        sum_row = sum(abs(A[i][j]) for j in range(n) if j != i)
        if abs(A[i][i]) < sum_row:
            diag_dom = False
            break
    if not diag_dom:
        print("The convergence condition for the Jacobi method IS NOT met.")
    else:
        print("The convergence condition for the Jacobi method IS met.")

    # iterations
    print("\n Iteration process:")
    print(f"Initial guess: x(0) = {[f'{v:.3f}' for v in x]}")

    for k in range(max_iter + 1):
        print(f"\n Iteration {k}:")
        for i in range(n):
            s = sum(A[i][j] * x[j] for j in range(n) if j != i)
            x_new[i] = (b[i] - s) / A[i][i]
            terms = " + ".join([f"{A[i][j]:.3f} * {x[j]:.3f}" for j in range(n) if j != i])
            print(f"x{i+1}({k}) = (b{i+1} - ({terms})) / {A[i][i]:.3f} "
                  f"= ({b[i]:.3f} - {s:.3f}) / {A[i][i]:.3f} = {x_new[i]:.6f}")
            
        # compute infinity norm of the difference
        diff = max(abs(x_new[i] - x[i]) for i in range(n))
        print(f"||dx|| = {diff:.3f}")
        
        # termination condition check
        if diff < eps:
            print(f"\nConvergence achieved after {k} iterations (eps = {eps}).")
            x = x_new.copy()
            break
        else:
            print("\nConvergence NOT achieved.")
        
        x = x_new.copy()

    # final result
    print("\n Final solution:")
    for i in range(n):
        print(f"x{i + 1} = {x[i]:.6f}")

    return x

if __name__ == "__main__":
    # Gaussian elimination
    A1 = [
        [1.0, 2.0, 3.0, 0.0],
        [4.0, 3.0, 1.0, 2.0],
        [2.0, 1.0, 2.0, 1.0],
        [0.0, 3.0, 0.0, -5.0]
    ]
    b1 = [22.0, 30.0, 21.0, -21.0]
    x1, _, inv_A1 = gaussian_elimination(A1, b1)
    # checking the result (inv(A) * A_orig)
    A1_check = [
        [1.0, 2.0, 3.0, 0.0],
        [4.0, 3.0, 1.0, 2.0],
        [2.0, 1.0, 2.0, 1.0],
        [0.0, 3.0, 0.0, -5.0]
    ]
    check_matrix = mat_mul(A1_check, inv_A1)
    print_matrix(check_matrix, b=[0]*4, title="A * A_inv :")

    # Tridiagonal (Thomas) method
    A2 = [
        [3.0, 2.0, 0.0],
        [2.0, 4.0, 1.0],
        [0.0, 1.0, 5.0]
    ]
    b2 = [9.0, 19.0, 28.0]
    x2 = tridiagonal_matrix_algorithm(A2, b2)

    # Yacobi method
    A3 = [
        [6.0, 0.0, 2.0, 3.0],
        [0.0, 4.0, 2.0, 1.0],
        [2.0, 2.0, 5.0, 0.0],
        [1.0, 1.0, 0.0, 3.0]
    ]
    b3 = [24.0, 18.0, 21.0, 15.0]
    x3 = jacobi_method(A3, b3, eps = 1e-3, max_iter = 50)