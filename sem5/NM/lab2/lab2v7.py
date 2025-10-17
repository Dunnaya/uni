def print_matrix(A, b, title = ""):
    print(f"\n{title}")
    n = len(A)
    for i in range(n):
        row = "".join(f"{A[i][j]:8.3f}" for j in range(n))
        print(f"[{row} | {b[i]:8.3f}]")
    print("-" * (10 * n + 6))

def gaussian_elimination(A, b):
    n = len(A)
    A = [row[:] for row in A]  # make a copy of A
    b = b[:]  # make a copy of b
    swaps = 0 # count of row swaps

    # store original matrix for inverse calculation
    A_orig = [row[:] for row in A]

    # add identity matrix for inverse calculation
    inv_A = [[1.0 if i == j else 0.0 for j in range(n)] for i in range(n)]

    print_matrix(A, b, " Initial Matrix:")

    # forward elimination
    for k in range(n):
        # pivoting
        print(f"Step {k + 1}: Pivoting for column {k + 1}")
        max_row = max(range(k, n), key = lambda j: abs(A[j][k]))
        pivot = A[max_row][k]
        print(f"Pivot element: A[{max_row + 1}, {k + 1}] = {pivot:.3f}")
        if (abs(A[max_row][k]) < 1e-12):
            raise ValueError("Matrix is singular")
        
        # rows swap
        if max_row != k:
            print(f"Swapping rows {k + 1} and {max_row + 1}")
            A[k], A[max_row] = A[max_row], A[k]
            b[k], b[max_row] = b[max_row], b[k]
            inv_A[k], inv_A[max_row] = inv_A[max_row], inv_A[k]
            swaps += 1
            print_matrix(A, b, f" After swapping rows:")
        
        # elimination
        for i in range(k + 1, n):
            factor = A[i][k] / A[k][k]
            print(f"Eliminating A[{i + 1}, {k + 1}], factor = {factor:.3f}")
            for j in range(k, n):
                A[i][j] -= factor * A[k][j]
            b[i] -= factor * b[k]
            # inverse matrix update
            for j in range(n):
                inv_A[i][j] -= factor * inv_A[k][j]
        print_matrix(A, b, f" After elimination in column {k + 1}:")
        
    # back substitution
    print("\nBack substitution:")
    x = [0.0] * n
    for i in range(n - 1, -1, -1):
        s = sum(A[i][j] * x[j] for j in range(i + 1, n))
        x[i] = (b[i] - s) / A[i][i]
        print(f"x{i + 1} = ({b[i]:.4f} - {s:.4f}) / {A[i][i]:.4f} = {x[i]:.6f}")
    
    print("\nFinal solution:")
    for i, val in enumerate(x, start = 1):
        print(f"x{i} = {val:.6f}")
    print(f"Total row swaps: {swaps}")

    # determinant calculation
    det = 1.0
    for i in range(n):
        det *= A[i][i]
    det *= (-1)**swaps
    print(f"\nDeterminant of the matrix: {det:.6f}")

    # inverse matrix calculation
    for k in range(n - 1, -1, -1):
        pivot = A[k][k]
        for j in range(n):
            inv_A[k][j] /= pivot
        A[k][k] = 1.0
        for i in range(k):
            factor = A[i][k]
            for j in range(n):
                inv_A[i][j] -= factor * inv_A[k][j]
            A[i][k] = 0.0
    print_matrix(inv_A, b = [0] * n, title = " Inverse Matrix:")

    # checking the result: inv(A) * A_orig should be identity
    result = [[0.0 for _ in range(n)] for _ in range(n)]
    for i in range(n):
        for j in range(n):
            result[i][j] = sum(inv_A[i][k] * A_orig[k][j] for k in range(n))
    print_matrix(result, b = [0] * n, title = " Inverse * initial:")

    return x, swaps, inv_A


def tridiagonal_matrix_algorithm(A, f):
    n = len(A)
    a = [0.0] + [A[i][i - 1] for i in range(1, n)] # sub-diagonal
    c = [A[i][i] for i in range(n)] # main diagonal
    b = [A[i][i + 1] for i in range(n - 1)] + [0.0] # super-diagonal (upper)

    print("\nTridiagonal Matrix Algorithm (Thomas Method):")
    print_matrix(A, f, " Initial Tridiagonal Matrix:")

    # stability check
    print(" Checking stability condition:")
    stable = all(abs(c[i]) >= abs(a[i]) + abs(b[i]) for i in range(n))
    if not stable:
        print("The stability condition of the Thomas method IS NOT met.")
    else:
        print("The stability condition of the Thomas method IS met.")
    
    # forward sweep
    print("\n Forward Sweep:")
    alpha = [0.0] * n
    beta = [0.0] * n
    z = [0.0] * n

    alpha[0] = -b[0] / c[0]
    beta[0] = f[0] / c[0]
    z[0] = c[0]
    print(f"i = 1: α1 = {-b[0]:.3f} / {c[0]:.3f} = {alpha[0]:.3f}, "
          f"β1 = {f[0]:.3f} / {c[0]:.3f} = {beta[0]:.3f}")

    for i in range(1, n):
        z[i] = c[i] + a[i] * alpha[i - 1]
        alpha[i] = -b[i] / z[i]
        beta[i] = (f[i] - a[i] * beta[i - 1]) / z[i]
        print(f"i = {i+1}: z{i+1} = {c[i]:.3f} + ({a[i]:.3f}) * ({alpha[i-1]:.3f}) = {z[i]:.3f}")
        print(f"      α{i+1} = {-b[i]:.3f} / {z[i]:.3f} = {alpha[i]:.3f}")
        print(f"      β{i+1} = ({f[i]:.3f} - {a[i]:.3f} * {beta[i-1]:.3f}) / {z[i]:.3f} = {beta[i]:.3f}")

    # back substitution
    print("\n Back Substitution:")
    x = [0.0] * n
    x[- 1] = beta[- 1]
    print(f"x{n} = β{n} = {x[-1]:.6f}")
    for i in range(n - 2, -1, -1):
        x[i] = alpha[i] * x[i + 1] + beta[i]
        print(f"x{i+1} = α{i+1} * x{i+2} + β{i+1} = "
              f"{alpha[i]:.3f} * {x[i+1]:.3f} + {beta[i]:.3f} = {x[i]:.6f}")

    # final solution
    print("\n Final solution:")
    for i in range(n):
        print(f"x{i + 1} = {x[i]:.6f}")

    return x

def jacobi_method(A, b, eps = 1e-3, max_iter = 100):
    n = len(A)
    x = [0.0] * n
    x_new = [0.0] * n

    print("\nJacobi Method:")
    print_matrix(A, b, " Initial Matrix:")

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
        print(f"‖Δx‖∞ = {diff:.3f}")
        
        # termination condition check
        if diff < eps:
            print(f"\nConvergence achieved after {k} iterations (ε = {eps}).")
            break
        else:
            print("\nConvergence NOT achieved.")
        
        x = x_new.copy()

    # final result
    print("\n Final solution:")
    for i in range(n):
        print(f"x{i + 1} = {x_new[i]:.6f}")

    return x_new

if __name__ == "__main__":
    # Gaussian elimination
    A1 = [
        [1.0, 2.0, 3.0, 0.0],
        [4.0, 3.0, 1.0, 2.0],
        [2.0, 1.0, 2.0, 1.0],
        [0.0, 3.0, 0.0, -5.0]
    ]
    b1 = [22.0, 30.0, 21.0, -21.0]
    x1, swaps1, inv_A1 = gaussian_elimination(A1, b1)

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