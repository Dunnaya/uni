import numpy as np
import matplotlib.pyplot as plt

def func(x):
    return 2 * x**8 + 3 * x**7 + 5 * x**5 - 2

def deriv_func(x):
    return 16 * x**7 + 21 * x**6 + 25 * x**4

# nodes
x_nodes = np.array([1, 3, 5], dtype = float)
y_nodes = func(x_nodes)
x_dense = np.linspace(1, 5, 200) # for plotting

print(" Input data:")
print(f"Function: 2*x^8 + 3*x^7 + 5*x^5 - 2")
print(f"Nodes x: {x_nodes}")
print(f"Values y: {y_nodes}\n")

# quadratic spline
def solve_quadratic_spline(bc_val, bc_name):
    print(f"\n\tBuilding spline ({bc_name})")
    n = len(x_nodes) - 1
    
    # calculate steps h
    print(" Calculate steps h_i")
    h = np.diff(x_nodes)
    for i, hi in enumerate(h):
        print(f"h[{i+1}] = {hi}")

    # form system
    # unknowns: a_i, b_i, c_i for each interval / total 3*n variables
    # S_i(x) = a_i + b_i(x-xi) + c_i(x-xi)^2
    print("\n Form system of equations for coefficients a, b, c")
    N_matrix = 3 * n
    A = np.zeros((N_matrix, N_matrix))
    B = np.zeros(N_matrix)
    
    row = 0
    # interpolation and continuity equations
    for i in range(n):
        hi = h[i]
        # Si(xi) = yi  => ai = yi
        A[row, 3*i] = 1
        B[row] = y_nodes[i]
        row += 1
        
        # Si(xi+1) = yi+1 => ai + bi*h + ci*h^2 = yi+1
        A[row, 3*i] = 1
        A[row, 3*i+1] = hi
        A[row, 3*i+2] = hi**2
        B[row] = y_nodes[i+1]
        row += 1

    # smoothness condition (derivatives) at internal nodes
    # Si'(xi+1) = S_{i+1}'(xi+1) => bi + 2*ci*hi - b_{i+1} = 0
    for i in range(n - 1):
        hi = h[i]
        A[row, 3*i + 1] = 1
        A[row, 3*i + 2] = 2 * hi
        A[row, 3*(i+1) + 1] = -1
        B[row] = 0
        row += 1
        
    # boundary condition at the start: S0'(x0) = given value
    # b0 = bc_val
    A[row, 1] = 1
    B[row] = bc_val
    
    print(f"Formed matrix of size {N_matrix}x{N_matrix}")
    print(f"Boundary condition: y'(1) = {bc_val}")

    # solution
    coeffs = np.linalg.solve(A, B)
    coeffs = coeffs.reshape(n, 3) # a, b, c for each interval
    
    print("\n Found coefficients")
    print(f"{'i':<3} | {'Interval':<10} | {'ai':<12} | {'bi':<12} | {'ci':<12}")
    print("-" * 60)
    for i in range(n):
        ai, bi, ci = coeffs[i]
        print(f"{i+1:<3} | [{x_nodes[i]}, {x_nodes[i+1]}] | {ai:<12.2f} | {bi:<12.2f} | {ci:<12.2f}")

    # output formulas
    print("\n Spline polynomials")
    for i in range(n):
        ai, bi, ci = coeffs[i]
        print(f"S_{i+1}(x) = {ai:.2f} + {bi:.2f}(x - {x_nodes[i]}) + {ci:.2f}(x - {x_nodes[i]})^2")
        
    return coeffs

# 1) natural spline (derivative = 0, according to condition)
coeffs_nat = solve_quadratic_spline(0, "Natural: y'(1)=0")

# 2) spline with exact derivative
true_deriv = deriv_func(x_nodes[0])
coeffs_exact = solve_quadratic_spline(true_deriv, f"Exact: y'(1)={true_deriv}")

# piecewise linear
print("\n Piecewise linear interpolation")
# L(x) = kx + b 
# k = (y2 - y1) / (x2 - x1)
# b = y1 - k * x1

linear_funcs = [] # for storing (k, b) for plotting
for i in range(len(x_nodes) - 1):
    x_start, x_end = x_nodes[i], x_nodes[i+1]
    y_start, y_end = y_nodes[i], y_nodes[i+1]
    
    print(f"\nInterval [{x_start}, {x_end}]")
    
    # slope k
    k = (y_end - y_start) / (x_end - x_start)
    print(f"k = ({y_end:.2f} - {y_start:.2f}) / ({x_end} - {x_start}) = {k:.4f}")
    
    # free term b
    b = y_start - k * x_start
    print(f"b = {y_start:.2f} - {k:.4f} * {x_start} = {b:.4f}")
    
    print(f"Result: L_{i+1}(x) = {k:.4f} * x + ({b:.4f})")
    linear_funcs.append((k, b))

# piecewise quadratic (Lagrange)
print("\n Piecewise quadratic interpolation (Lagrange)")
# since there are only 3 points build a single 2nd degree Lagrange polynomial over the entire interval [1, 5]
# L2(x) = y0*L0 + y1*L1 + y2*L2

print(f"\nNodes: {x_nodes}")
print("Building a single Lagrange polynomial for the interval [1, 5] (3 points)")
# calculating coefficients a*x^2 + b*x + c
# using polyfit for accurate determination of a, b, c of the polynomial
poly_coeffs = np.polyfit(x_nodes, y_nodes, 2)
a_lagr, b_lagr, c_lagr = poly_coeffs

print(f"\nCoefficients of polynomial P(x) = ax^2 + bx + c:")
print(f"a (coefficient of x^2) = {a_lagr:.4f}")
print(f"b (coefficient of x)   = {b_lagr:.4f}")
print(f"c (constant term) = {c_lagr:.4f}")
print(f"\nResult: P_2(x) = {a_lagr:.4f}x^2 + {b_lagr:.4f}x + {c_lagr:.4f}")

# plots
def eval_spline(x_arr, x_n, coeffs):
    y_res = []
    for x in x_arr:
        # find interval
        idx = np.searchsorted(x_n, x, side = 'right') - 1
        if idx >= len(coeffs): idx = len(coeffs) - 1
        if idx < 0: idx = 0
        
        xi = x_n[idx]
        ai, bi, ci = coeffs[idx]
        val = ai + bi*(x - xi) + ci*(x - xi)**2
        y_res.append(val)
    return np.array(y_res)

def eval_linear(x_arr, x_n, lin_funcs):
    y_res = []
    for x in x_arr:
        idx = np.searchsorted(x_n, x, side = 'right') - 1
        if idx >= len(lin_funcs): idx = len(lin_funcs) - 1
        if idx < 0: idx = 0
        k, b_val = lin_funcs[idx]
        y_res.append(k * x + b_val)
    return np.array(y_res)

# calculating values for plotting
y_s_nat = eval_spline(x_dense, x_nodes, coeffs_nat)
y_s_exact = eval_spline(x_dense, x_nodes, coeffs_exact)
y_lin = eval_linear(x_dense, x_nodes, linear_funcs)
y_quad = np.polyval(poly_coeffs, x_dense)
y_true = func(x_dense)

plt.figure(figsize = (14, 12))

# plot 1: splines (log scale)
plt.subplot(2, 1, 1)
plt.semilogy(x_dense, y_true, 'k--', linewidth = 2, label = 'Exact function')
plt.semilogy(x_dense, y_s_nat, 'g-', label = 'Spline (y\'=0)')
plt.semilogy(x_dense, y_s_exact, 'r-.', label = f'Spline (y\'={true_deriv})')
plt.scatter(x_nodes, y_nodes, color = 'black', s = 80, zorder = 5, label = 'Nodes')
plt.title('Task 1: Quadratic Splines (Log Scale)')
plt.xlabel('x')
plt.ylabel('y')
plt.legend()
plt.grid(True, which = "both", alpha = 0.5)

# plot 2: piecewise (log scale)
plt.subplot(2, 1, 2)
plt.semilogy(x_dense, y_true, 'k--', linewidth = 2, label = 'Exact function')
plt.semilogy(x_dense, y_lin, 'b-', label = 'Piecewise Linear')
plt.semilogy(x_dense, y_quad, 'm:', linewidth = 2, label = 'Piecewise Quadratic')
plt.scatter(x_nodes, y_nodes, color = 'black', s = 80, zorder = 5, label = 'Nodes')
plt.title('Task 2: Piecewise Interpolations (Log Scale)')
plt.xlabel('x')
plt.ylabel('y')
plt.legend()
plt.grid(True, which = "both", alpha = 0.5)

plt.tight_layout()
plt.savefig('result.png')
print("\nPlots have been generated and saved to 'result.png'.")
plt.show()