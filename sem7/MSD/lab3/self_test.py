import numpy as np
from dct import C, dct_forward, dct_inverse
from dwt import dwt_forward, dwt_inverse
from compression import compress
from metrics import psnr

rng = np.random.default_rng(0)
img = rng.integers(0, 256, (64, 64)).astype(float)

# 1. ортогональність матр. DCT
assert np.allclose(C @ C.T, np.eye(8)), "C не ортогональна"
print("OK 1: C @ C.T == I")

# 2. DC-коеф. сталого блоку
flat = np.full((8, 8), 200.0)
d = dct_forward(flat)[0, 0]
assert np.isclose(d[0, 0], (200 - 128) * 8) and np.allclose(d.ravel()[1:], 0)
print("OK 2: сталий блок -> лише DC =", d[0, 0])

# 3. ідеальне відновлення без стиснення
assert np.allclose(dct_inverse(dct_forward(img)), img)
assert np.allclose(dwt_inverse(dwt_forward(img, 3), 3), img)
print("OK 3: DCT і DWT відновлюють зображення точно (keep=100%)")

# 4. ортонормованість
assert np.isclose(np.sum(dwt_forward(img, 3) ** 2), np.sum(img ** 2))
print("OK 4: енергія Хаара збережена")

# 5. порівн. з бібліотекою
try:
    from scipy.fft import dctn
    b = img[:8, :8] - 128
    assert np.allclose(dct_forward(img)[0, 0], dctn(b, norm="ortho"))
    print("OK 5: збігається зі scipy.fft.dctn")
except ImportError:
    print("-- 5: scipy не встановлено, пропущено")

# 6. к-сть ненульових після стиснення
c = compress(dct_forward(img), 0.1)
print("OK 6: ненульових", np.count_nonzero(c), "з", c.size, "(~10%)")