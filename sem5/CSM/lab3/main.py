import numpy as np
import csv
from datetime import datetime
import time
import matplotlib.pyplot as plt
import os

def ensure_logs_directory():
    """ creates logs directory if it doesn't exist """
    if not os.path.exists('logs'):
        os.makedirs('logs')

def read_file(file_name):
    """ reads data from file, returns it as a transposed numpy array """
    with open(file_name, 'r') as file:
        lines = file.readlines()
        input_data = []
        for line in lines:
            values = line.strip().split()
            if values:  # check for empty lines
                row = [float(value) for value in values]
                input_data.append(row)
    return np.array(input_data).T

def finite_diff(y_vec_func, b_vec, b_vals, delta = 1e-5):
    """ calculates the derivative matrix using finite differences """
    n = len(y_vec_func(b_vals))
    m = len(b_vec)
    deriv_matrix = np.zeros((n, m))
    
    for j in range(m):
        original_value = b_vals[b_vec[j]]
        b_vals[b_vec[j]] = original_value + delta
        y_plus = y_vec_func(b_vals)
        b_vals[b_vec[j]] = original_value - delta
        y_minus = y_vec_func(b_vals)
        b_vals[b_vec[j]] = original_value
        deriv_matrix[:, j] = (y_plus - y_minus) / (2 * delta)
    
    return deriv_matrix

def get_u(a_matr, b_matr, u_matr, h):
    """ Runge-Kutta method for the sensitivity matrix """
    b_arrayed = np.array(b_matr)
    k1 = h * (np.dot(a_matr, u_matr) + b_arrayed)
    k2 = h * (np.dot(a_matr, u_matr + k1 / 2) + b_arrayed)
    k3 = h * (np.dot(a_matr, u_matr + k2 / 2) + b_arrayed)
    k4 = h * (np.dot(a_matr, u_matr + k3) + b_arrayed)
    return u_matr + (k1 + 2 * k2 + 2 * k3 + k4) / 6

def get_y(a_matr, y_cur, h):
    """ Runge-Kutta method for the system state """
    k1 = h * np.dot(a_matr, y_cur)
    k2 = h * np.dot(a_matr, y_cur + k1 / 2)
    k3 = h * np.dot(a_matr, y_cur + k2 / 2)
    k4 = h * np.dot(a_matr, y_cur + k3)
    return y_cur + (k1 + 2 * k2 + 2 * k3 + k4) / 6

def init_matr(params):
    """ initializes the system matrix based on physical parameters """
    # parameters: c1..c4 (stiffness), m1..m3 (masses)
    c1, c2, c3, c4 = params['c1'], params['c2'], params['c3'], params['c4']
    m1, m2, m3 = params['m1'], params['m2'], params['m3']
    
    matr = [
        [0, 1, 0, 0, 0, 0],
        [-(c2 + c1) / m1, 0, c2 / m1, 0, 0, 0],
        [0, 0, 0, 1, 0, 0],
        [c2 / m2, 0, -(c2 + c3) / m2, 0, c3 / m2, 0],
        [0, 0, 0, 0, 0, 1],
        [0, 0, c3 / m3, 0, -(c4 + c3) / m3, 0]
    ]
    return np.array(matr)

def get_model_solution(params, y0, t_points, h = 0.2):
    """ calculates the model solution over the time interval """
    a_matrix = init_matr(params)
    y_current = y0
    y_solution = [y0]
    
    for _ in range(len(t_points) - 1):
        y_current = get_y(a_matrix, y_current, h)
        y_solution.append(y_current)
    
    return np.array(y_solution)

def approximate(y_matr, params, beta_symbols, beta_vals, eps, h = 0.2):
    """ main approximation loop to identify parameters """
    ensure_logs_directory()
    start_time = time.time()
    
    timestamp = datetime.now().strftime("%Y%m%d_%H%M%S")
    log_filename = os.path.join('logs', f'approximation_log_{timestamp}.csv')
    
    with open(log_filename, 'w', newline = '') as csvfile:
        writer = csv.writer(csvfile)
        headers = ['iteration'] + beta_symbols + ['quality_degree']
        writer.writerow(headers)
    
    all_params = {**params, **beta_vals}
    
    beta_vector = np.array([beta_vals[symbol] for symbol in beta_symbols])
    iteration = 0
    
    def y_vec_func(b_vals):
        all_params.update(b_vals)
        a_matrix = init_matr(all_params)
        return a_matrix @ y_approximation
    
    while True:
        all_params.update(beta_vals)
        a_complete = init_matr(all_params)
        
        u_matr = np.zeros((6, len(beta_symbols)))
        quality_degree = 0
        integral_part_inverse = np.zeros((len(beta_symbols), len(beta_symbols)))
        integral_part_mult = np.zeros((1, len(beta_symbols)))
        y_approximation = y_matr[0]
        
        for i in range(len(y_matr)):
            b_derivative_matr = finite_diff(y_vec_func, beta_symbols, beta_vals)
            integral_part_inverse += u_matr.T @ u_matr
            integral_part_mult += u_matr.T @ (y_matr[i] - y_approximation)
            quality_degree += (y_matr[i] - y_approximation).T @ (y_matr[i] - y_approximation)
            
            u_matr = get_u(a_complete, b_derivative_matr, u_matr, h)
            y_approximation = get_y(a_complete, y_approximation, h)
        
        integral_part_inverse *= h
        integral_part_mult *= h
        quality_degree *= h
        
        with open(log_filename, 'a', newline = '') as csvfile:
            writer = csv.writer(csvfile)
            writer.writerow([iteration] + list(beta_vector) + [quality_degree])
        
        try:
            delta_beta = np.linalg.inv(integral_part_inverse) @ integral_part_mult.flatten()
        except np.linalg.LinAlgError:
            print(" Error: Singular matrix. Check initial approximations or data.")
            break

        beta_vector += delta_beta
        
        beta_vals = {beta_symbols[i]: beta_vector[i] for i in range(len(beta_symbols))}
        
        print(f"Iteration {iteration}: Quality = {quality_degree:.6e}")

        if quality_degree < eps or iteration > 100:
            end_time = time.time()
            execution_time = end_time - start_time
            
            summary_filename = os.path.join('logs', f'summary_{timestamp}.txt')
            with open(summary_filename, 'w') as f:
                f.write("Parameter Identification Results:\n")
                f.write("\nIdentified parameters:\n")
                for param, value in beta_vals.items():
                    f.write(f"{param}: {value:.6f}\n")
                f.write(f"\nQuality indicator: {quality_degree:.6e}\n")
                f.write(f"Total iterations: {iteration + 1}\n")
                f.write(f"Execution time: {execution_time:.2f} seconds\n")
            
            return beta_vals, iteration + 1, execution_time, quality_degree
            
        iteration += 1

def plot_results(measured_data, model_solution, t_points, save_prefix):
    """ plots the comparison between measured data and model solution """
    ensure_logs_directory()
    variables = ['x1', 'dx1/dt', 'x2', 'dx2/dt', 'x3', 'dx3/dt']
    fig, axes = plt.subplots(3, 2, figsize=(15, 12))
    axes = axes.flatten()
    
    min_len = min(len(t_points), len(measured_data), len(model_solution))
    
    for i, (ax, var) in enumerate(zip(axes, variables)):
        if i < measured_data.shape[1]:
            ax.plot(t_points[:min_len], measured_data[:min_len, i], 'r.', label='Measured', markersize=4)
            ax.plot(t_points[:min_len], model_solution[:min_len, i], 'b-', label='Model')
            ax.set_title(f'Variable {var}')
            ax.set_xlabel('Time')
            ax.set_ylabel('Value')
            ax.grid(True)
            ax.legend()
    
    plt.tight_layout()
    plot_path = os.path.join('logs', f'{save_prefix}_comparison.png')
    plt.savefig(plot_path)
    plt.close()

if __name__ == "__main__":
    ensure_logs_directory()
    
    filename = './data/y8.txt'
    
    print(f" Reading data from {filename}...")
    try:
        input_data = read_file(filename)
    except FileNotFoundError:
        print(f" Error: File {filename} not found.")
        exit(1)

    # step size h (usually 0.2)
    h_step = 0.2 
    t_points = np.arange(0, h_step * len(input_data), h_step)
    
    params = {
        'c1': 0.14,
        'c2': 0.3,
        'c4': 0.12,
        'm1': 12
    }
    
    to_approx = {
        'm2': 30,
        'm3': 23,
        'c3': 0.1
    }
    
    approx_keys = list(to_approx.keys())
    
    print("\nKnown parameters:", params)
    print("Parameters to estimate (initial guess):", to_approx)

    # run approximation
    result, iterations, execution_time, quality = approximate(
        input_data, params, approx_keys, to_approx, 1e-6, h = h_step
    )
    
    final_params = {**params, **result}
    
    # get model solution with identified parameters
    model_solution = get_model_solution(final_params, input_data[0], t_points, h = h_step)
    
    timestamp = datetime.now().strftime("%Y%m%d_%H%M%S")
    
    print("\n Identified parameters:")
    for param, value in result.items():
        print(f"{param}: {value:.6f}")
    
    print("\n Performance metrics:")
    print(f"Quality indicator: {quality:.6e}")
    print(f"Total iterations: {iterations}")
    print(f"Execution time: {execution_time:.2f} seconds")
    
    plot_results(input_data, model_solution, t_points, f'results_{timestamp}')
    print(f"\nResults saved to 'logs' directory")