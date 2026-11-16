import numpy as np

np.set_printoptions(precision=4, suppress=True)

A = np.array([[0.15, 0.55, 0.10],
              [0.35, 0.35, 0.20],
              [0.15, 0.15, 0.20]])
E = np.eye(3)
print("Матриця A:\n", A)

# характеристичний поліном det(mu*E - A) = mu^3 + c1*mu^2 + c2*mu + c3
coef = np.poly(A)
print("\nКоефіцієнти характеристичного полінома (за спаданням степенів):", coef)
print("mu^3 %+.4f*mu^2 %+.4f*mu %+.4f = 0" % tuple(coef[1:]))
print("Слід A =", np.trace(A), " (= -c1)")

# власні числа, число Фробеніуса
lam, X = np.linalg.eig(A)
print("\nВласні числа A:", lam)
i = np.argmax(lam.real)
lamA = lam[i].real
print("Число Фробеніуса λ_A =", lamA)

# правий вектор Фробеніуса (повний випуск), нормування за мінімальним
xv = np.abs(X[:, i].real)
xA = xv / xv.min()
print("\nПравий вектор Фробеніуса x_A =", xA)

# лівий вектор Фробеніуса (ціни): власний вектор A^T
lam1, Pm = np.linalg.eig(A.T)
j = np.argmax(lam1.real)
pv = np.abs(Pm[:, j].real)
pA = pv / pv.min()
print("Лівий вектор Фробеніуса p_A =", pA, " (λ для A^T =", lam1[j].real, ")")

# продуктивність
print("\nλ_A < 1 ?", lamA < 1, "=> матриця", "продуктивна" if lamA < 1 else "непродуктивна")

# матриця повних витрат
B = np.linalg.inv(E - A)
print("\nМатриця повних витрат B = (E-A)^-1:\n", B)
print("B >= 0 ?", bool((B >= 0).all()))

# збіжність ряду E + A + A^2 + ... + A^N до B (|b_ij - s_ij| < 0.01)
print("\nЗбіжність ряду S_N = E + A + ... + A^N до B:")
S = E.copy()
Ak = E.copy()
n = 0
print(f"{'N':>3} {'max|B-S_N|':>12}")
print(f"{n:>3} {np.abs(B - S).max():>12.5f}")
while np.abs(B - S).max() >= 0.01:
    n += 1
    Ak = Ak @ A
    S = S + Ak
    print(f"{n:>3} {np.abs(B - S).max():>12.5f}")
print(f"\nПерший N з |b_ij - s_ij| < 0.01 для всіх i,j: N = {n}")
print("S_N =\n", S)
print("B - S_N =\n", B - S)

# модель цін: p = s B  (s - додана вартість в цінах)
s = np.array([0.25, 0.35, 0.30])
p = s @ B
print("\nДодана вартість s =", s)
print("Вектор цін p = s B =", p)
print("Перевірка: p A + s =", p @ A + s)