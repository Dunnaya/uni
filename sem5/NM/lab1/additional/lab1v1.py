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

# relaxation method
def relaxation_method(coeffs, interval, accuracy=1e-3, factor=0.5):
    a, b = interval
    if evaluate_poly(coeffs, a) * evaluate_poly(coeffs, b) >= 0:
        raise ValueError("Invalid interval: no roots")
    
    x = (a+b)/2
    max_iter = int(math.log2((b-a)/accuracy)) + 1
    decimal_places = -int(math.log10(accuracy))
    
    print(f"\nSolving {poly_to_string(coeffs)} using relaxation with accuracy {accuracy}")
    print(f"Interval: {interval}")
    print(f"Initial guess: {x:.{decimal_places}f}")
    print("Step\tApproximation\tFunction Value")
    
    for i in range(max_iter):
        prev = x
        x = prev - factor * evaluate_poly(coeffs, prev)
        print(f"{i}\t{x:.{decimal_places}f}\t{evaluate_poly(coeffs, x):.{decimal_places}f}")
        if abs(x - prev) < accuracy:
            break
    return x

# modified Newton method
def modified_newton_method(coeffs, interval, accuracy=1e-3):
    a, b = interval
    if evaluate_poly(coeffs, a) * evaluate_poly(coeffs, b) >= 0:
        raise ValueError("Invalid interval: no roots")
    
    x = (a+b)/2
    deriv_coeffs = derivative_poly(coeffs)
    deriv_at_guess = evaluate_poly(deriv_coeffs, x)
    max_iter = int(math.log2((b-a)/accuracy)) + 1
    decimal_places = -int(math.log10(accuracy))
    
    print(f"\nSolving {poly_to_string(coeffs)} using modified Newton with accuracy {accuracy}")
    print(f"Interval: {interval}")
    print(f"Initial guess: {x:.{decimal_places}f}")
    print("Step\tApproximation\tFunction Value")
    
    for i in range(max_iter):
        prev = x
        x = prev - evaluate_poly(coeffs, prev)/deriv_at_guess
        print(f"{i}\t{x:.{decimal_places}f}\t{evaluate_poly(coeffs, x):.{decimal_places}f}")
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

# interface for user input
def interactive_test(method, coeffs, default_interval, default_accuracy=1e-3):
    interval_input = input(f"\nEnter interval {default_interval} (format a,b) or press enter: ")
    interval = tuple(map(float, interval_input.split(","))) if interval_input else default_interval

    accuracy_input = input(f"Enter desired accuracy {default_accuracy} or press enter: ")
    accuracy = float(accuracy_input) if accuracy_input else default_accuracy

    if method == "relaxation":
        solution = relaxation_method(coeffs, interval, accuracy)
    else:
        solution = modified_newton_method(coeffs, interval, accuracy)

    print(f"Solution found: {solution:.{-int(math.log10(accuracy))}f} with accuracy {accuracy}")
    return solution

if __name__ == "__main__":
    # solving the equations
    print("\n Relaxation Method:")
    interactive_test("relaxation", [-2, -1, 2, 1], (-1.5, -0.5))

    print("\n Modified Newton Method:")
    interactive_test("modified_newton", [2, -1, -2, 1], (0.5, 1.556))

    # plots
    plot_polynomial([-2, -1, 2, 1], (-2, 2), "first_plot.png", "x^3 + 2x^2 - x - 2")
    plot_polynomial([2, -1, -2, 1], (0, 2), "second_plot.png", "x^3 - 2x^2 - x + 2")