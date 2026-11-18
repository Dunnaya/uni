import numpy as np


def compress(coeffs, keep, protect=None):
    mags = np.abs(coeffs).astype(float)
    if protect is not None:
        mags[protect] = np.inf
    k = max(1, int(round(keep * coeffs.size)))
    thr = np.partition(mags.ravel(), -k)[-k]      # k-те за величиною значення
    return np.where(mags >= thr, coeffs, 0.0)