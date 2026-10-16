import numpy as np
import matplotlib
matplotlib.use('Agg')
import matplotlib.pyplot as plt
from scipy.integrate import solve_ivp
from scipy.optimize import brentq

# Варіант N = 11
N = 11
r = 3 * N            # N in [9;16]
q = 300 * N
k = r / q
p_crit = r * q / 4
print("r =", r, " q =", q, " rq/4 =", p_crit, " k = r/q =", k)

def f(x, p):
    return r * x * (1 - x / q) - p

# випадок 1: p = rq/4
p1 = p_crit
x1 = q / 2
print("\nВипадок 1: p =", p1, " x* =", x1, " D =", q**2 - 4 * p1 * q / r)

# випадок 2: p = rq/4 - a^2
a2 = 10000
p2 = p_crit - a2
D2 = q**2 - 4 * p2 * q / r
xs2 = sorted(np.roots([1, -q, p2 * q / r]).real)
eps2 = np.sqrt(D2) / 2
print("\nВипадок 2: p =", p2, " D =", D2, " x1,x2 =", xs2, " eps =", eps2)

# випадок 3: p = rq/4 + a^2
p3 = p_crit + a2
D3 = q**2 - 4 * p3 * q / r
eps3 = np.sqrt(-D3) / 2
print("\nВипадок 3: p =", p3, " D =", D3, " eps =", eps3)

# точні розв'язки
def sol1(t, x0):
    u0 = x0 - q / 2
    return q / 2 + u0 / (1 + k * u0 * t)

def sol2(t, x0):
    u0 = x0 - q / 2
    e = eps2
    z = u0 / e
    if abs(z) < 1:
        return q / 2 + e * np.tanh(k * e * t + np.arctanh(z))
    c = 0.5 * np.log((z + 1) / (z - 1))      # arcoth
    return q / 2 + e / np.tanh(k * e * t + c)

def sol3(t, x0):
    u0 = x0 - q / 2
    return q / 2 + eps3 * np.tan(np.arctan(u0 / eps3) - k * eps3 * t)

def num(p, x0, T):
    ev = lambda t, x: x[0]
    ev.terminal = True
    ev.direction = -1
    s = solve_ivp(lambda t, x: [f(x[0], p)], [0, T], [x0], rtol=1e-11, atol=1e-9,
                  events=ev, dense_output=True)
    return s

cases = [
    (1, p1, sol1, [1000, 2500], 1.0),
    (2, p2, sol2, [300, 1500, 3000], 0.6),
    (3, p3, sol3, [q / 3, 3 * q / 4], 0.25),
]

def t_zero(sol, x0, tmax):
    g = lambda t: sol(t, x0)
    ts = np.linspace(0, tmax, 200001)
    with np.errstate(all='ignore'):
        v = np.array([g(t) for t in ts[::100]])
    return v

res = {}
for n, p, sol, x0s, T in cases:
    print("\n випадок", n)
    for x0 in x0s:
        s = num(p, x0, T)
        tz = s.t_events[0][0] if len(s.t_events[0]) else None
        t_ = 0.5 if tz is None else tz * 0.5
        an = float(sol(t_, x0))
        nm = float(s.sol(t_)[0])
        end = s.y[0][-1]
        print(f"x0={x0:.1f}: t_extinct={tz}, x(t={t_:.4f}) формула={an:.4f} чисельно={nm:.4f}, x(T)={end:.2f}")
        res[(n, x0)] = tz

# графіки
plt.rcParams['font.size'] = 10
def plot_case(n, p, sol, x0s, T, eq, fname, title):
    plt.figure(figsize=(7, 4.2))
    cols = ['tab:red', 'tab:green', 'tab:blue']
    for x0, c in zip(x0s, cols):
        s = num(p, x0, T)
        tt = np.linspace(0, s.t[-1], 800)
        xx = s.sol(tt)[0]
        plt.plot(tt, xx, c, label=f'x(0) = {x0:.0f}')
        plt.plot(0, x0, 'o', color=c)
        if len(s.t_events[0]):
            plt.plot(s.t_events[0][0], 0, 'x', color=c, markersize=9)
    for e in eq:
        plt.axhline(e, color='gray', linestyle='--', linewidth=1)
        plt.text(T * 1.005, e, f'{e:.0f}', va='center', fontsize=8)
    plt.axhline(q, color='k', linestyle=':', linewidth=1)
    plt.text(T * 1.005, q, f'q={q}', va='center', fontsize=8)
    plt.xlim(0, T)
    plt.ylim(-80, q + 200)
    plt.xlabel('t')
    plt.ylabel('x(t)')
    plt.title(title)
    plt.grid(alpha=.4)
    plt.legend(loc='best')
    plt.tight_layout()
    plt.savefig(fname, dpi=150)
    plt.close()

plot_case(1, p1, sol1, [1000, 2500], 1.0, [x1], 'c1.png', f'Випадок 1: p = {p1:.2f}')
plot_case(2, p2, sol2, [300, 1500, 3000], 0.5, xs2, 'c2.png', f'Випадок 2: p = {p2:.2f}')
plot_case(3, p3, sol3, [q / 3, 3 * q / 4], 0.25, [], 'c3.png', f'Випадок 3: p = {p3:.2f}')

xs = np.linspace(0, q, 400)
plt.figure(figsize=(7, 4))
for p, lab in ((p1, 'p = rq/4'), (p2, 'p = rq/4 - a²'), (p3, 'p = rq/4 + a²')):
    plt.plot(xs, f(xs, p), label=f'{lab} = {p:.0f}')
plt.axhline(0, color='k', linewidth=.8)
plt.xlabel('x'); plt.ylabel('dx/dt'); plt.title('Права частина рівняння f(x)')
plt.grid(alpha=.4); plt.legend(); plt.tight_layout()
plt.savefig('fx.png', dpi=150); plt.close()