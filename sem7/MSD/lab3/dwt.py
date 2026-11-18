"""Власна реалізація багаторівневого DWT Хаара."""
import numpy as np

S = np.sqrt(2)


def _fwd_1d_rows(x):
    a, b = x[:, 0::2], x[:, 1::2]
    return np.hstack(((a + b) / S, (a - b) / S))      # [низькі | високі]


def _inv_1d_rows(x):
    half = x.shape[1] // 2
    L, H = x[:, :half], x[:, half:]
    out = np.empty_like(x)
    out[:, 0::2] = (L + H) / S
    out[:, 1::2] = (L - H) / S
    return out


def dwt_forward(img, levels=3):
    out = img.astype(float).copy()
    h, w = out.shape
    for _ in range(levels):
        sub = out[:h, :w]
        sub = _fwd_1d_rows(sub)           # по рядках
        sub = _fwd_1d_rows(sub.T).T       # по стовпцях
        out[:h, :w] = sub
        h //= 2
        w //= 2
    return out


def dwt_inverse(coeffs, levels=3):
    out = coeffs.astype(float).copy()
    H, W = out.shape
    for lvl in range(levels - 1, -1, -1):
        h, w = H >> lvl, W >> lvl
        sub = out[:h, :w]
        sub = _inv_1d_rows(sub.T).T
        sub = _inv_1d_rows(sub)
        out[:h, :w] = sub
    return out


def ll_mask(shape, levels=3):
    m = np.zeros(shape, dtype=bool)
    m[: shape[0] >> levels, : shape[1] >> levels] = True
    return m