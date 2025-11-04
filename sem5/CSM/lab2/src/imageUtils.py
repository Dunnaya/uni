import cv2
import numpy as np
import os
import matplotlib.pyplot as plt

def readImg():
    x_img = cv2.imread("../data/x3.bmp", cv2.IMREAD_GRAYSCALE)
    y_img = cv2.imread("../data/y10.bmp", cv2.IMREAD_GRAYSCALE)

    x = x_img.astype(float) / 255.0
    y = y_img.astype(float) / 255.0

    print(f"x size: {x.shape}, y size: {y.shape}")
    print(f"x range: [{x.min():.3f}, {x.max():.3f}]")
    print(f"y range: [{y.min():.3f}, {y.max():.3f}]")

    return x, y

def saveImg(matrix, title = "Result", filename = None):
    matrix_display = np.clip(matrix, 0, 1)

    plt.figure(figsize=(8, 6))
    plt.imshow(matrix_display, cmap = 'gray', vmin = 0, vmax = 1)
    plt.title(title)
    plt.axis('off')
    plt.colorbar(label = 'Intensity')

    if filename:
        os.makedirs("results", exist_ok=True)
        plt.savefig(os.path.join("results", filename), bbox_inches = 'tight', dpi = 150)

    plt.close()