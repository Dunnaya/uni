import numpy as np
import matplotlib.pyplot as plt

def func(x):
    return 2 * x**8 + 3 * x**7 + 5 * x**5 - 2

def get_max_derivative_5(x):
    """
    5th deriv (n = 4 -> n + 1 deriv)

    f(x) = 2x^8 + 3x^7 + 5x^5 - 2
    f'(x) = 16x^7 + 21x^6 + 25x^4
    f''(x) = 112x^6 + 126x^5 + 100x^3
    f'''(x) = 672x^5 + 630x^4 + 300x^2
    f(4)(x) = 3360x^4 + 2520x^3 + 600x
    f(5)(x) = 13440x^3 + 7560x^2 + 600
    
    max on [1, 5] is at x = 5 (function is increasing)
    """
    return 13440 * x**3 + 7560 * x**2 + 600

def nodes_Chebyshov(func, nodes_num, a, b):
    # x_k = (a+b)/2 + (b-a)/2 * cos((2k+1)pi / 2(n+1))
    print(f" Choosing {nodes_num} nodes by Chebyshov polynomial on [{a}, {b}]")

    nodes = []
    print(f"{'k':<3} {'xk':<10} {'f(xk)':<10}")
    for k in range(nodes_num):
        # formula for k = 0..n (where n + 1 is num of nodes)
        xk = (a + b) / 2 + ((b - a) / 2) * np.cos((2 * k + 1) * np.pi / (2 * nodes_num))
        nodes.append(xk)
        print(f"{k:<3} {xk:<10.4f} {func(xk):<10.4f}")
    return nodes

def lagrange_basis(x, k, nodes):
    # Lagrange basis polynomial l_k(x) calc
    l_k = 1.0
    for i in range(len(nodes)):
        if i != k:
            l_k *= (x - nodes[i]) / (nodes[k] - nodes[i])
    return l_k

def lagrange_polynomial_eval(nodes, y_nodes, x):
    # calc the val of the Lagrange polynomial at pt. x
    # L_n(x) = sum(y_k * l_k(x))
    val = 0.0
    for k in range(len(nodes)):
        val += y_nodes[k] * lagrange_basis(x, k, nodes)
    return val

def lagrange_polynomial_string(nodes, y_nodes):
    # form string representation of the polynomial (simplified)
    terms = []
    for k in range(len(nodes)):
        term = f"{y_nodes[k]:.4f} * l_{k}(x)"
        terms.append(term)
    return " + ".join(terms)

def factorial_func(n):
    res = 1
    for i in range(1, n + 1):
        res *= i
    return res

def interpolation_error(func, lagrange_poly_func, max_der_func, a, b, nodes, test_x):
    # error estimation: |f(x) - Ln(x)| <= M_(n+1)/(n+1)! * (b-a)^(n+1) / 2^(2n+1) for Chebyshev nodes
    n = len(nodes) - 1 # polynomial degree
    y_nodes = [func(n) for n in nodes]
    
    # max deriv on the interval (taking the right edge as the deriv is asc)
    M_n_plus_1 = max_der_func(b) 
    
    # theoretical error for Chebyshev nodes 
    leq = (M_n_plus_1 / factorial_func(n + 1)) * (((b - a)**(n + 1)) / (2**(2 * n + 1)))
    
    func_res = func(test_x)
    poly_res = lagrange_poly_func(nodes, y_nodes, test_x)
    
    print("\n Interpolation Error Estimation")
    print(f"Test point x              = {test_x}")
    print(f"Function value f(x)       = {func_res:.6f}")
    print(f"Lagrange polynomial L(x)  = {poly_res:.6f}")
    print(f"Absolute difference       = |f(x) - L(x)| = {abs(func_res - poly_res):.6f}")
    print(f"Theoretical error bound   = {leq:.6f}")

    if abs(func_res - poly_res) <= leq:
        print("\nInterpolation error is within the theoretical bound.")
    else:
        print("\nNote: Error might exceed bound if test point is outside [a,b] (extrapolation).")

def find_root_theoretical(func, left, right, eps = 1e-5):
    # finding root by bisection method (for verification)
    print(f"\n Searching for theoretical root on [{left}, {right}]")
    
    if func(left) * func(right) > 0:
        print("Warn: No sign change detected on interval. Root might be outside.")
        # try to find a wider interval with a sign change for demonstration
        if func(0) * func(1) < 0:
             print("  -> Switch to [0, 1] where root exists for f(x)=2x^8+3x^7+5x^5-2")
             left, right = 0, 1
        else:
             return None

    iteration = 0
    while right - left > eps:
        mid = (left + right) / 2
        if func(left) * func(mid) < 0:
            right = mid
        else:
            left = mid
        iteration += 1
    
    root = (left + right) / 2
    print(f"Theoretical Root x ≈ {root:.10f}")
    return root

def direct_interpolation(func, nodes_num, a, b, eps = 1e-5):
    # direct interpolation for root finding (solving L_n(x) = 0)
    print("\n Direct Interpolation Method")
    # [1, 5]
    nodes = nodes_Chebyshov(func, nodes_num, a, b)
    y_nodes = [func(x) for x in nodes]
    
    def P(x):
        return lagrange_polynomial_eval(nodes, y_nodes, x)

    # root is located between 0 and 1 (f(0)=-2, f(1)=8)
    # and the interpolation interval is [1, 5] so we will perform EXTRAPOLATION (hell nah)..

    # to find the root we change the search interval to [0, 1] using the polynomial built on [1, 5]
    
    search_a, search_b = a, b
    print(f"\nSearching for root of L(x) = 0 on [{search_a}, {search_b}]")

    fa = P(search_a)
    fb = P(search_b)
    
    if fa * fb > 0:
        print("Error: No sign change for polynomial on search interval.")
        return None

    # bisection method on P(x)
    left, right = search_a, search_b
    iteration = 0
    while right - left > eps:
        mid = (left + right) / 2
        fmid = P(mid)
        if fmid == 0: break
        if P(left) * fmid < 0:
            right = mid
        else:
            left = mid
        iteration += 1

    root = (left + right) / 2
    print(f"Root found by Direct Interpolation: x ≈ {root:.10f}")
    print(f"Check f(root) ≈ {func(root):.10f}")
    return root

def inverse_interpolation(func, nodes_num, a, b):
    # swap x and y, build L(y), find value at y=0
    print("\n Inverse Interpolation Method")
    
    # nodes on [1, 5]
    x_nodes = nodes_Chebyshov(func, nodes_num, a, b)
    y_nodes = [func(x) for x in x_nodes]

    # on [1, 5] the function is monotonic
    
    print("Building polynomial x(y) (swapping nodes)...")
    # finding x when y = 0
    
    root = lagrange_polynomial_eval(y_nodes, x_nodes, 0) # x(0)

    print(f"Root found by Inverse Interpolation: x ≈ {root:.10f}")
    print(f"Check f(root) ≈ {func(root):.10f}")
    
    # since the root (≈0.7) is far from the interpolation nodes [1, 5] this is extrapolation, so the accuracy may be low
    return root

def plot_interpolation_analysis(func, nodes, y_nodes, a, b):
    x_dense = np.linspace(a, b, 200)
    y_true = func(x_dense)
    y_lagrange = [lagrange_polynomial_eval(nodes, y_nodes, xi) for xi in x_dense]
    error = np.abs(y_true - y_lagrange)

    plt.figure(figsize = (12, 10))

    # plot 1: function and polynomial
    plt.subplot(2, 1, 1)
    plt.plot(x_dense, y_true, 'b-', label = 'Original Function $f(x)$', linewidth = 2)
    plt.plot(x_dense, y_lagrange, 'r--', label = 'Lagrange Polynomial $L(x)$', linewidth = 2)
    plt.scatter(nodes, y_nodes, color = 'black', s = 50, zorder = 5, label = 'Chebyshev Nodes')
    plt.title(f'Interpolation on [{a}, {b}]')
    plt.xlabel('x')
    plt.ylabel('y')
    plt.legend()
    plt.grid(True)

    # plot 2: error
    plt.subplot(2, 1, 2)
    plt.plot(x_dense, error, 'g-', label = 'Absolute Error $|f(x) - L(x)|$')
    plt.scatter(nodes, [0]*len(nodes), color = 'black', s = 30, label = 'Nodes (Error = 0)')
    plt.title('Interpolation Error')
    plt.xlabel('x')
    plt.ylabel('Error')
    plt.legend()
    plt.grid(True)

    plt.tight_layout()
    plt.savefig('interpolation_analysis.png')
    print("\n Plot saved as 'interpolation_analysis.png'")
    plt.show()

def plot_root_search(func, nodes, y_nodes, a, b, search_range_start):
    x_wide = np.linspace(search_range_start, b, 200)
    y_true_wide = func(x_wide)
    y_lagrange_wide = [lagrange_polynomial_eval(nodes, y_nodes, xi) for xi in x_wide]

    plt.figure(figsize = (10, 6))
    plt.plot(x_wide, y_true_wide, 'b-', label = 'Original Function $f(x)$', alpha = 0.6)
    plt.plot(x_wide, y_lagrange_wide, 'r--', label = 'Lagrange Polynomial $L(x)$')
    
    plt.axvspan(a, b, color = 'yellow', alpha = 0.1, label = 'Interpolation Interval')
    
    # Y = 0 line
    plt.axhline(0, color = 'black', linewidth = 1)
    
    plt.scatter(nodes, y_nodes, color = 'black', zorder = 5, label = 'Nodes')
    
    # plt.ylim(-100, 5000) 
    
    plt.title('Task 2: Root Finding & Polynomial Behavior')
    plt.xlabel('x')
    plt.ylabel('y')
    plt.legend()
    plt.grid(True)
    plt.savefig('root_search.png')
    print("\n Plot saved as 'root_search.png'")
    plt.show()

if __name__ == "__main__":
    nodes_num = 5
    
    # CHANGED INTERVAL TO [0, 1] to include the root (approx 0.76) for higher accuracy
    start = 0.5  # was 1
    end = 1     # was 5

    nodes = nodes_Chebyshov(func, nodes_num, start, end)
    y_vals = [func(x) for x in nodes]
    
    print(f"\nLagrange Polynomial constructed on [{start}, {end}] with {nodes_num} nodes.")
    
    test_x = 0.5
    interpolation_error(func, lagrange_polynomial_eval, get_max_derivative_5, start, end, nodes, test_x)
    
    plot_interpolation_analysis(func, nodes, y_vals, start, end)

    theoretical_root = find_root_theoretical(func, -1, 2) # finding the exact root for comparison

    direct_interpolation(func, nodes_num, start, end)

    inverse_interpolation(func, nodes_num, start, end)

    plot_root_search(func, nodes, y_vals, start, end, search_range_start = -0.2)