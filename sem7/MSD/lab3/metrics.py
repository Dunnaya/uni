import numpy as np


def psnr(orig, rec):
    mse = np.mean((orig.astype(float) - rec.astype(float)) ** 2)
    if mse == 0:
        return float("inf")
    return 10 * np.log10(255.0 ** 2 / mse)


def nonzero_count(coeffs):
    return int(np.count_nonzero(coeffs))


def compression_ratio(coeffs):
    return coeffs.size / max(1, nonzero_count(coeffs))