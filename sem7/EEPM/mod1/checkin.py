# швидка перевірка розв'язків (запускати окремо, у звіт не вставляти)
import numpy as np
from scipy.integrate import solve_ivp

print("--- Задача 1 ---")
K = 180
r = 0.004
for P0 in (250, 120):
    P = lambda t: K * P0 / (P0 + (K - P0) * np.exp(r * K * t))
    t = 0.5
    h = 1e-6
    dP = (P(t + h) - P(t - h)) / (2 * h)       # похідна чисельно
    print("P0 =", P0, " P(0) =", P(0), " dP/dt - r*P*(P-K) =", round(dP - r * P(t) * (P(t) - K), 6), " P(1) =", round(P(1), 3))

print("--- Задача 2 ---")
b = [0, 0, 0.19, 0.44, 0.5, 0.5, 0.45]
for s, s7 in ((0.87, 0.8), (0.8, 0.75)):
    L = np.zeros((7, 7))
    L[0, :] = b
    for i in range(6):
        L[i + 1, i] = s
    L[6, 6] = s7
    lam = max(np.linalg.eigvals(L).real)
    # просто множимо вектор на матрицю багато разів
    x = np.ones(7)
    for k in range(3000):
        new = L @ x
        ratio = new.sum() / x.sum()
        x = new / new.sum()
    print("lambda з eig =", round(lam, 5), " з циклу =", round(ratio, 5), " H =", round((1 - 1 / lam) * 100, 3))
    print("структура, %:", np.round(x * 100, 2))

print("--- Задача 3 ---")
f = lambda x, y: 3 * x - 2 * x * y - 0.5 * x**2
g = lambda x, y: -y + 3 * x * y
for p in [(0, 0), (6, 0), (1 / 3, 17 / 12)]:
    print("точка", p, " f, g в ній:", round(f(*p), 10), round(g(*p), 10))
sol = solve_ivp(lambda t, z: [f(*z), g(*z)], [0, 200], [2, 1], rtol=1e-10)
print("модель 2, кінцева точка:", sol.y[:, -1].round(4), " (очікуємо 0.3333 1.4167)")