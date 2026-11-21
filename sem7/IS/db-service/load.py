import logging
import random
import threading
import time

import psycopg2

import db
from metrics import observe

log = logging.getLogger("load")

MAX_ROWS = 50_000
STATUSES = ["new", "paid", "shipped", "done", "cancelled"]
CUSTOMERS = ["Olena", "Ivan", "Maria", "Taras", "Sofia", "Andrii", "Kateryna", "Dmytro"]


def do_insert(cur):
    cur.execute(
        "INSERT INTO orders (customer, amount, status) VALUES (%s, %s, %s)",
        (random.choice(CUSTOMERS), round(random.uniform(5, 500), 2), "new"),
    )


def do_update(cur):
    cur.execute(
        """
        UPDATE orders SET status = %s
        WHERE id = (SELECT id FROM orders ORDER BY random() LIMIT 1)
        """,
        (random.choice(STATUSES),),
    )


def do_delete(cur):
    cur.execute(
        "DELETE FROM orders WHERE id = (SELECT id FROM orders ORDER BY random() LIMIT 1)"
    )


def do_select(cur):
    cur.execute("SELECT * FROM orders WHERE status = %s LIMIT 100", (random.choice(STATUSES),))
    cur.fetchall()


def do_aggregate(cur):
    cur.execute("SELECT customer, count(*), sum(amount) FROM orders GROUP BY customer")
    cur.fetchall()


def trim_table(cur):
    cur.execute("SELECT count(*) FROM orders")
    total = cur.fetchone()[0]
    if total > MAX_ROWS:
        cur.execute(
            "DELETE FROM orders WHERE id IN (SELECT id FROM orders ORDER BY id LIMIT %s)",
            (total - MAX_ROWS,),
        )
        log.info("Видалено %d старих рядків", total - MAX_ROWS)


ACTIONS = [
    ("insert", do_insert, 50),
    ("update", do_update, 20),
    ("delete", do_delete, 8),
    ("select", do_select, 17),
    ("aggregate", do_aggregate, 5),
]


def run_load_loop(stop: threading.Event):
    # це з'єднання не трасуємо, щоб не засмічувати Tempo
    names = [a[0] for a in ACTIONS]
    funcs = {a[0]: a[1] for a in ACTIONS}
    weights = [a[2] for a in ACTIONS]

    conn = db.connect(traced=False)
    cur = conn.cursor()
    db.init_schema(cur)

    step = 0
    while not stop.is_set():
        name = random.choices(names, weights)[0]
        try:
            with observe(name):
                funcs[name](cur)
            step += 1
            if step % 1000 == 0:
                trim_table(cur)
                log.info("Виконано %d операцій", step)
            time.sleep(random.uniform(0.02, 0.25))
        except psycopg2.Error as exc:
            log.error("Помилка БД: %s, перепідключення", str(exc).strip())
            try:
                conn.close()
            except Exception:
                pass
            conn = db.connect(traced=False)
            cur = conn.cursor()
            db.init_schema(cur)
