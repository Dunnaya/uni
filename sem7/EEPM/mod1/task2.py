import numpy as np
import matplotlib.pyplot as plt

# Задача 2. Модель Леслі, варіант 2
# 7 вікових груп (0-2, 2-4, ..., 10-12, 12+), крок 2 роки
b = [0, 0, 0.19, 0.44, 0.5, 0.5, 0.45]     # вектор народжуваності

# Матриця Леслі: перший рядок - народжуваність, піддіагональ - коефіцієнти
# переходу в наступну групу, у правому нижньому куті - виживання в старшій групі
def matrix(s, s_last):
    L = np.zeros((7, 7))
    L[0, :] = b
    for i in range(6):
        L[i + 1, i] = s
    L[6, 6] = s_last
    return L

# швидкість росту = найбільше власне значення lambda,
# стійка вікова структура = відповідний власний вектор
def solve(L):
    values, vectors = np.linalg.eig(L)
    k = np.argmax(values.real)
    lam = values[k].real
    x = np.abs(vectors[:, k].real)
    x = x / x.sum() * 100        # переводимо у відсотки
    return lam, x

L1 = matrix(0.87, 0.8)     # початкові умови
L2 = matrix(0.8, 0.75)     # після змін

res = []
for name, L in (("До змін", L1), ("Після змін", L2)):
    lam, x = solve(L)
    H = (1 - 1 / lam) * 100      # частка, яку можна виловити
    print(name)
    print(L)
    print("lambda =", round(lam, 5))
    print("приріст за 2 роки:", round((lam - 1) * 100, 2), "%")
    print("приріст за рік:", round((np.sqrt(lam) - 1) * 100, 2), "%")
    print("структура, %:", np.round(x, 2))
    print("перевірка L*x - lam*x:", np.abs(L @ x - lam * x).max().round(10))
    print("H =", round(H, 2), "%")
    print()
    res.append(x)

# графік: порівняння стійкої вікової структури
groups = ['0-2', '2-4', '4-6', '6-8', '8-10', '10-12', '12+']
n = np.arange(7)
plt.figure(figsize=(8, 4.5))
plt.bar(n - 0.2, res[0], 0.4, label='До змін')
plt.bar(n + 0.2, res[1], 0.4, label='Після змін')
plt.xticks(n, groups)
plt.xlabel('Вік, роки')
plt.ylabel('Частка, %')
plt.title('Стійка вікова структура')
plt.legend()
plt.grid(axis='y')
plt.tight_layout()
plt.savefig('lesli.png', dpi=150)