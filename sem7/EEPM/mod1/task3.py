import numpy as np
import matplotlib.pyplot as plt
from scipy.integrate import solve_ivp

# Задача 3. Хижак-жертва, варіант 3
# модель 1:  x' = x(e1 - g1*y),         y' = y(-e2 + g2*x)
# модель 2:  x' = x(e1 - g1*y - beta*x), y' = y(-e2 + g2*x)
e1 = 3      # розмноження жертв
e2 = 1      # загибель хижаків
g1 = 2      # жертви зменшуються
g2 = 3      # хижаки зростають
beta = 0.5  # конкуренція серед жертв

# ---------- стаціонарні точки і їх тип ----------
# точки знайшли вручну з умови x' = 0, y' = 0:
# (0, 0); (e2/g2, e1/g1) для моделі 1; ще (e1/beta, 0) для моделі 2,
# де в моделі 2 y = (e1*g2 - beta*e2)/(g1*g2)
points1 = [(0, 0), (e2 / g2, e1 / g1)]
points2 = [(0, 0), (e1 / beta, 0), (e2 / g2, (e1 * g2 - beta * e2) / (g1 * g2))]

# матриці Якобі
def jac1(x, y):
    return np.array([[e1 - g1 * y, -g1 * x],
                     [g2 * y, -e2 + g2 * x]])

def jac2(x, y):
    return np.array([[e1 - g1 * y - 2 * beta * x, -g1 * x],
                     [g2 * y, -e2 + g2 * x]])

# тип точки за власними значеннями матриці Якобі
def point_type(ev):
    re = ev.real
    im = ev.imag
    if abs(im).max() < 1e-9:                # власні значення дійсні
        if re.max() > 0 and re.min() < 0:
            return "сідло (нестійка)"
        if re.max() < 0:
            return "стійкий вузол"
        return "нестійкий вузол"
    if abs(re).max() < 1e-9:                # суто уявні
        return "центр"
    if re.max() < 0:
        return "стійкий фокус"
    return "нестійкий фокус"

for name, points, jac in (("Модель 1", points1, jac1), ("Модель 2", points2, jac2)):
    print(name)
    for px, py in points:
        ev = np.linalg.eigvals(jac(px, py))
        print("точка", (round(px, 4), round(py, 4)), " власні значення:", np.round(ev, 4), " -", point_type(ev))
    print()

# ---------- розв'язання системи ----------
def model1(t, z):
    X, Y = z
    return [e1 * X - g1 * X * Y, -e2 * Y + g2 * X * Y]

def model2(t, z):
    X, Y = z
    return [e1 * X - g1 * X * Y - beta * X**2, -e2 * Y + g2 * X * Y]

def run(model, z0, T):
    t = np.linspace(0, T, 3000)
    s = solve_ivp(model, [0, T], z0, t_eval=t, rtol=1e-10, atol=1e-12)
    return s.t, s.y[0], s.y[1]

start_a = (2, 1)    # x0 > y0
start_b = (1, 2)    # x0 < y0

# перевірка: у моделі 1 величина 3x - ln x + 2y - 3 ln y має бути сталою
for z0 in (start_a, start_b):
    t, X, Y = run(model1, z0, 10)
    H = g2 * X - e2 * np.log(X) + g1 * Y - e1 * np.log(Y)
    print("модель 1, старт", z0, " H max-min =", H.max() - H.min())

# модель 2 має прийти в точку (1/3; 17/12)
for z0 in (start_a, start_b):
    t, X, Y = run(model2, z0, 150)
    print("модель 2, старт", z0, " кінець:", round(X[-1], 4), round(Y[-1], 4))

# ---------- графіки ----------
def phase_portrait(model, T, xmax, ymax, points, name, title, others):
    plt.figure(figsize=(7, 6))
    xs, ys = np.meshgrid(np.linspace(0.02, xmax, 25), np.linspace(0.02, ymax, 25))
    u, v = model(0, [xs, ys])
    length = np.hypot(u, v)
    plt.quiver(xs, ys, u / length, v / length, color='lightgray', pivot='mid')
    for z0 in others:
        t, X, Y = run(model, z0, T)
        plt.plot(X, Y, color='steelblue', linewidth=0.8)
    for z0, color, label in ((start_a, 'red', 'а) x0>y0'), (start_b, 'green', 'б) x0<y0')):
        t, X, Y = run(model, z0, T)
        plt.plot(X, Y, color=color, linewidth=2, label=label)
        plt.plot(z0[0], z0[1], 'o', color=color)
    for p in points:
        plt.plot(p[0], p[1], 'kx', markersize=10)
    plt.xlim(-0.1, xmax)
    plt.ylim(-0.1, ymax)
    plt.xlabel('Жертви x')
    plt.ylabel('Хижаки y')
    plt.title(title)
    plt.legend(loc='upper right')
    plt.grid()
    plt.tight_layout()
    plt.savefig(name, dpi=140)
    plt.close()

phase_portrait(model1, 10, 3, 4, [(0, 0), (e2 / g2, e1 / g1)], 'm1_phase.png',
               'Модель 1: фазовий портрет', [(0.5, 0.5), (1, 1), (0.2, 1.5), (1.5, 2.5)])
phase_portrait(model2, 12, 7, 4, [(0, 0), (e1 / beta, 0), (e2 / g2, 17 / 12)], 'm2_phase.png',
               'Модель 2: фазовий портрет', [(0.5, 0.5), (4, 1), (6, 3), (0.2, 3)])

for tag, model, T in (('m1', model1, 10), ('m2', model2, 15)):
    # графіки x(t), y(t)
    plt.figure(figsize=(12, 4.2))
    for i, (z0, title) in enumerate(((start_a, 'а) x0>y0: (2; 1)'), (start_b, 'б) x0<y0: (1; 2)'))):
        t, X, Y = run(model, z0, T)
        plt.subplot(1, 2, i + 1)
        plt.plot(t, X, 'g', label='Жертви x(t)')
        plt.plot(t, Y, 'r', label='Хижаки y(t)')
        plt.title(title)
        plt.xlabel('t')
        plt.ylabel('Чисельність')
        plt.grid()
        plt.legend()
    plt.tight_layout()
    plt.savefig(tag + '_dyn.png', dpi=140)
    plt.close()

    # 3D графіки
    fig = plt.figure(figsize=(12, 5))
    for i, (z0, title) in enumerate(((start_a, 'а) x0>y0: (2; 1)'), (start_b, 'б) x0<y0: (1; 2)'))):
        t, X, Y = run(model, z0, T)
        ax = fig.add_subplot(1, 2, i + 1, projection='3d')
        ax.plot(X, Y, t, color='purple')
        ax.set_xlabel('Жертви x')
        ax.set_ylabel('Хижаки y')
        ax.set_zlabel('t')
        ax.set_title(title)
    plt.tight_layout()
    plt.savefig(tag + '_3d.png', dpi=140)
    plt.close()