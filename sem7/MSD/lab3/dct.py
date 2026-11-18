import numpy as np

N = 8


def dct_matrix(n=N):
    C = np.zeros((n, n))
    for i in range(n):
        for j in range(n):
            a = np.sqrt(1 / n) if i == 0 else np.sqrt(2 / n)
            C[i, j] = a * np.cos((2 * j + 1) * i * np.pi / (2 * n))
    return C


C = dct_matrix()


def to_blocks(img):
    h, w = img.shape
    return img.reshape(h // N, N, w // N, N).transpose(0, 2, 1, 3)


def from_blocks(blocks):
    bh, bw = blocks.shape[:2]
    return blocks.transpose(0, 2, 1, 3).reshape(bh * N, bw * N)


def dct_forward(img):
    blocks = to_blocks(img.astype(float) - 128)   # зсув рівня
    return C @ blocks @ C.T                        # matmul працює одразу для всіх блоків


def dct_inverse(coeffs):
    blocks = C.T @ coeffs @ C
    return from_blocks(blocks) + 128