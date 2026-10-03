import numpy as np
import matplotlib.pyplot as plt
from scipy.integrate import solve_ivp

# Задача 1. Ізольована популяція, варіант 2
# dP/dt = 0.004 * P * (P - 180),  P(0) = 250 та P(0) = 120
r = 0.004
K = 180

# Стаціонарні точки: dP/dt = 0 => P = 0 і P = 180
# при P < 180 похідна від'ємна (популяція спадає), при P > 180 додатна (зростає)
# тобто P = 0 стійка, P = 180 нестійка
print("Стаціонарні точки: P = 0 (стійка), P = 180 (нестійка)")

# Розв'язок (відокремлення змінних, початкова умова P(0) = P0):
# P(t) = K*P0 / (P0 + (K - P0)*exp(r*K*t))
def P(t, P0):
    return K * P0 / (P0 + (K - P0) * np.exp(r * K * t))

# чисельність при t = 1
for P0 in (250, 120):
    print("P0 =", P0, " P(1) =", round(P(1, P0), 2))

# при P0 = 250 знаменник перетворюється в нуль - чисельність зростає до нескінченності
t_end = np.log(250 / (250 - K)) / (r * K)
print("для P0 = 250 розв'язок іде в нескінченність при t =", round(t_end, 3))

# перевірка: розв'язуємо рівняння чисельно і порівнюємо з формулою
for P0 in (250, 120):
    sol = solve_ivp(lambda t, p: r * p * (p - K), [0, 1], [P0], rtol=1e-10)
    print("перевірка чисельно, P0 =", P0, " P(1) =", round(sol.y[0][-1], 2))

# графіки
t1 = np.linspace(0, 1.75, 300)
t2 = np.linspace(0, 6, 300)

plt.figure(figsize=(12, 4.5))

plt.subplot(1, 2, 1)
plt.plot(t1, P(t1, 250), 'r', label='P(0) = 250')
plt.axhline(K, color='gray', linestyle='--', label='P = 180')
plt.axvline(t_end, color='r', linestyle=':')
plt.plot(1, P(1, 250), 'ko')
plt.ylim(0, 1000)
plt.title('P(0) = 250')
plt.xlabel('t, місяці')
plt.ylabel('P(t)')
plt.grid()
plt.legend()

plt.subplot(1, 2, 2)
plt.plot(t2, P(t2, 120), 'b', label='P(0) = 120')
plt.axhline(K, color='gray', linestyle='--', label='P = 180')
plt.plot(1, P(1, 120), 'ko')
plt.ylim(0, 200)
plt.title('P(0) = 120')
plt.xlabel('t, місяці')
plt.ylabel('P(t)')
plt.grid()
plt.legend()

plt.tight_layout()
plt.savefig('izol.png', dpi=150)