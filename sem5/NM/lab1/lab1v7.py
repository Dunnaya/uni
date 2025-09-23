import numpy as np
import math
import matplotlib.pyplot as plt

def evaluate_poly(coeffs, x):
    # evaluates a polynomial with given coefficients at x
    x = np.asarray(x)
    powers = np.arange(len(coeffs))
    return np.sum(coeffs * (x[..., None] ** powers), axis=-1)

def derivative_poly(coeffs):
    # returns the coefficients of the derivative of the polynomial
    return np.array([i*c for i, c in enumerate(coeffs)][1:])

def poly_to_string(coeffs):
    terms = []
    for i, c in enumerate(coeffs):
        if c == 0: continue
        if i == 0: term = f"{c}"
        elif i == 1: term = f"{c}x"
        else: term = f"{c}x^{i}"
        if c < 0: term = "-" + term[1:]
        terms.append(term)
    return " + ".join(terms) if terms else "0"

# newton method
def newton_method(coeffs, interval, accuracy=1e-3):
    a, b = interval
    if evaluate_poly(coeffs, a) * evaluate_poly(coeffs, b) >= 0:
        raise ValueError("Invalid interval: no roots")

    x = (a+b)/2
    deriv_coeffs = derivative_poly(coeffs)
    max_iter = int(math.log2((b-a)/accuracy)) + 1
    decimal_places = -int(math.log10(accuracy))

    print(f"\nSolving {poly_to_string(coeffs)} using Newton with accuracy {accuracy}")
    print(f"Interval: {interval}")
    print(f"Initial guess: {x:.{decimal_places}f}")
    print("Step\tApproximation\tFunction Value")

    for i in range(max_iter):
        prev = x
        f_val = evaluate_poly(coeffs, prev)
        f_der = evaluate_poly(deriv_coeffs, prev)
        if f_der == 0:
            raise ZeroDivisionError("Derivative became zero during iteration")
        x = prev - f_val / f_der
        print(f"{i}\t{x:.{decimal_places}f}\t\t{evaluate_poly(coeffs, x):.{decimal_places}f}")
        if abs(x - prev) < accuracy:
            break
    return x

# simple iteration method
def simple_iteration_method(coeffs, interval, accuracy=1e-3, factor=0.1):
    a, b = interval
    x = (a+b)/2   # initial guess
    max_iter = int(math.log2((b-a)/accuracy)) + 1
    decimal_places = -int(math.log10(accuracy))

    print(f"\nSolving {poly_to_string(coeffs)} using simple iteration with accuracy {accuracy}")
    print(f"Interval: {interval}")
    print(f"Initial guess: {x:.{decimal_places}f}")
    print("Step\tApproximation\tFunction Value")

    for i in range(max_iter):
        prev = x
        x = prev - factor * evaluate_poly(coeffs, prev)
        print(f"{i}\t{x:.{decimal_places}f}\t\t{evaluate_poly(coeffs, x):.{decimal_places}f}")
        if abs(x - prev) < accuracy:
            break
    return x


# plots
def plot_polynomial(coeffs, x_range, save_path, title):
    x_vals = np.linspace(*x_range, 1000)
    y_vals = evaluate_poly(coeffs, x_vals)
    plt.figure(figsize=(8,5))
    plt.plot(x_vals, y_vals, label=poly_to_string(coeffs))
    plt.axhline(0, color='black')
    plt.xlabel("x")
    plt.ylabel("f(x)")
    plt.title(title)
    plt.grid(True)
    plt.legend()
    plt.savefig(save_path)
    #plt.show()

# interface
def interactive_test(method, coeffs, default_interval, default_accuracy=1e-3):
    interval_input = input(f"\nEnter interval {default_interval} (format a,b) or press enter: ")
    interval = tuple(map(float, interval_input.split(","))) if interval_input else default_interval

    accuracy_input = input(f"Enter desired accuracy {default_accuracy} or press enter: ")
    accuracy = float(accuracy_input) if accuracy_input else default_accuracy

    if method == "newton":
        solution = newton_method(coeffs, interval, accuracy)
    else:
        solution = simple_iteration_method(coeffs, interval, accuracy)

    print(f"Solution found: {solution:.{-int(math.log10(accuracy))}f} with accuracy {accuracy}")
    return solution

if __name__ == "__main__":
    # equation 1: x^3 + x^2 - 4x - 4 = 0 (Newton)
    print("\n Newton Method:")
    interactive_test("newton", [-4, -4, 1, 1], (1, 3))

    # equation 2: x^3 - 4x^2 - 4x + 16 = 0 (Simple Iteration)
    print("\n Simple Iteration Method:")
    interactive_test("iteration", [16, -4, -4, 1], (1, 4))

    # plots
    plot_polynomial([-4, -4, 1, 1], (-2, 3), "newton_plot.png", "x^3 + x^2 - 4x - 4")
    plot_polynomial([16, -4, -4, 1], (-1, 5), "iteration_plot.png", "x^3 - 4x^2 - 4x + 16")