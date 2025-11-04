import numpy as np
import matplotlib.pyplot as plt
import time
import psutil
import os

from pseudoInverseMatrixMethods import pseudoInverseMatrix_Greville, pseudoInverseMatrix_MoorePenrose
from operationsCounter import count_operations_greville, count_operations_moore_penrose
from imageUtils import readImg

def calcError(Y, Y_pred):
    error = np.linalg.norm(Y - Y_pred, ord = 1)
    MSE = np.mean((Y - Y_pred)**2)
    RMSE = np.sqrt(MSE)
    return error, MSE, RMSE

def calcOperator(X, Y, inversion_func, op_counter, eps = 1e-6, delta = 1000):
    start_time = time.time()
    start_mem = psutil.Process(os.getpid()).memory_info().rss / 1024 / 1024

    print(f"Sizes: X = {X.shape}, Y = {Y.shape}")

    # find pseudoinverse of X
    X_pinv, iters = inversion_func(X, eps = eps, delta = delta)
    print(f" X_pinv shape: {X_pinv.shape}")

    # calculate operator A
    A = Y @ X_pinv
    print(f" A shape: {A.shape}")

    # calculate result
    Y_pred = A @ X
    print(f" Y_pred shape: {Y_pred.shape}")

    # clip values to the range [0, 1]
    Y_pred = np.clip(Y_pred, 0, 1)

    end_time = time.time()
    end_mem = psutil.Process(os.getpid()).memory_info().rss / 1024 / 1024

    time_used = end_time - start_time
    memory_used = end_mem - start_mem
    ops = op_counter(X.shape[0], X.shape[1], iters if iters is not None else 1)
    error, MSE, RMSE = calcError(Y, Y_pred)

    return Y_pred, time_used, memory_used, ops, error, MSE, RMSE

if __name__ == "__main__":
    X, Y = readImg()

    print(f"Matrix sizes: X = {X.shape}, Y = {Y.shape}")

    print("\n\tMoore-Penrose Method")
    Y_MP, time_MP, mem_MP, ops_MP, err_MP, MSE_MP, RMSE_MP = calcOperator(
        X, Y, pseudoInverseMatrix_MoorePenrose, count_operations_moore_penrose
    )
    print(f"Time: {time_MP:.4f}s, Memory: {mem_MP:.2f}MB, Operations: {ops_MP}, RMSE: {RMSE_MP:.6f}")

    print("\n\tGreville Method")
    Y_G, time_G, mem_G, ops_G, err_G, MSE_G, RMSE_G = calcOperator(
        X, Y, pseudoInverseMatrix_Greville, count_operations_greville
    )
    print(f"Time: {time_G:.4f}s, Memory: {mem_G:.2f}MB, Operations: {ops_G}, RMSE: {RMSE_G:.6f}")

    print("\n\tResults comparison")
    print(f"Time difference: {abs(time_MP - time_G):.4f}s ({('Moore-Penrose' if time_MP < time_G else 'Greville')} is faster)")
    print(f"Memory difference: {abs(mem_MP - mem_G):.2f}MB")
    print(f"RMSE difference: {abs(RMSE_MP - RMSE_G):.6f}")

    # results visualization
    fig = plt.figure(figsize = (18, 11))

    # original images
    plt.subplot(2, 5, 1)
    plt.imshow(X, cmap = 'gray', vmin = 0, vmax = 1)
    plt.title('Input image X')
    plt.axis('off')
    plt.colorbar(fraction = 0.045)

    plt.subplot(2, 5, 2)
    plt.imshow(Y, cmap = 'gray', vmin = 0, vmax = 1)
    plt.title('Expected image Y')
    plt.axis('off')
    plt.colorbar(fraction = 0.045)

    # method results
    plt.subplot(2, 5, 3)
    plt.imshow(Y_MP, cmap = 'gray', vmin = 0, vmax = 1)
    plt.title(f'Moore-Penrose\nRMSE: {RMSE_MP:.4f}')
    plt.axis('off')
    plt.colorbar(fraction = 0.045)

    plt.subplot(2, 5, 4)
    plt.imshow(Y_G, cmap = 'gray', vmin = 0, vmax = 1)
    plt.title(f'Greville\nRMSE: {RMSE_G:.4f}')
    plt.axis('off')
    plt.colorbar(fraction = 0.045)

    # difference between expected and obtained
    plt.subplot(2, 5, 5)
    diff_MP = np.abs(Y - Y_MP)
    plt.imshow(diff_MP, cmap = 'hot', vmin = 0, vmax = diff_MP.max())
    plt.title(f'Difference MP\nMax: {diff_MP.max():.4f}')
    plt.axis('off')
    plt.colorbar(fraction = 0.045)

    # comparison of metrics
    methods = ['Moore-Penrose', 'Greville']
    times = [time_MP, time_G]
    memories = [mem_MP, mem_G]
    rmses = [RMSE_MP, RMSE_G]
    operations = [ops_MP, ops_G]

    plt.subplot(2, 5, 6)
    bars = plt.bar(methods, times, color = ["#6cb0de", "#6cde92"])
    plt.title('Execution time')
    plt.ylabel('Seconds')
    plt.grid(axis = 'y', alpha = 0.3)
    for i, bar in enumerate(bars):
        height = bar.get_height()
        plt.text(bar.get_x() + bar.get_width()/2., height,
                f'{times[i]:.3f}s',
                ha = 'center', va = 'bottom', fontsize = 9)

    plt.subplot(2, 5, 7)
    bars = plt.bar(methods, memories, color = ["#6cb0de", "#6cde92"])
    plt.title('Memory usage')
    plt.ylabel('Megabytes')
    plt.grid(axis = 'y', alpha = 0.3)
    for i, bar in enumerate(bars):
        height = bar.get_height()
        plt.text(bar.get_x() + bar.get_width()/2., height,
                f'{memories[i]:.1f}MB',
                ha = 'center', va = 'bottom', fontsize = 9)

    plt.subplot(2, 5, 8)
    bars = plt.bar(methods, rmses, color = ["#6cb0de", "#6cde92"])
    plt.title('RMSE error')
    plt.ylabel('RMSE')
    plt.grid(axis = 'y', alpha = 0.3)
    for i, bar in enumerate(bars):
        height = bar.get_height()
        plt.text(bar.get_x() + bar.get_width()/2., height,
                f'{rmses[i]:.5f}',
                ha = 'center', va = 'bottom', fontsize = 9)

    plt.subplot(2, 5, 9)
    bars = plt.bar(methods, operations, color = ["#6cb0de", "#6cde92"])
    plt.title('Number of operations')
    plt.ylabel('Operations')
    plt.grid(axis = 'y', alpha = 0.3)
    for i, bar in enumerate(bars):
        height = bar.get_height()
        plt.text(bar.get_x() + bar.get_width()/2., height,
                f'{operations[i]:,}',
                ha = 'center', va = 'bottom', fontsize = 8, rotation = 0)

    plt.subplot(2, 5, 10)
    diff_G = np.abs(Y - Y_G)
    plt.imshow(diff_G, cmap = 'hot', vmin = 0, vmax = diff_G.max())
    plt.title(f'Difference Greville\nMax: {diff_G.max():.4f}')
    plt.axis('off')
    plt.colorbar(fraction = 0.045)

    plt.tight_layout()
    plt.savefig('../results/comparison_results.png', dpi = 150, bbox_inches = 'tight')
    print("\nThe plot is saved as 'comparison_results.png' in the results folder.")
    plt.show()