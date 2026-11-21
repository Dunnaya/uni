import logging
import os
import random
import time

import psycopg2

logging.basicConfig(level=logging.INFO, format="%(asctime)s %(levelname)s %(message)s")
log = logging.getLogger("generator")

DB = {
    "host": os.getenv("DB_HOST", "localhost"),
    "port": int(os.getenv("DB_PORT", "5432")),
    "dbname": os.getenv("DB_NAME", "appdb"),
    "user": os.getenv("DB_USER", "app"),
    "password": os.getenv("DB_PASSWORD", "app"),
}

MAX_ROWS = 50_000
STATUSES = ["new", "paid", "shipped", "done", "cancelled"]
CUSTOMERS = ["Olena", "Ivan", "Maria", "Taras", "Sofia", "Andrii", "Kateryna", "Dmytro"]


def connect():
    while True:
        try:
            conn = psycopg2.connect(**DB)
            conn.autocommit = True
            log.info("Підключено до %s:%s/%s", DB["host"], DB["port"], DB["dbname"])
            return conn
        except psycopg2.OperationalError as exc:
            log.warning("БД недоступна (%s), повтор через 3 с", str(exc).strip())
            time.sleep(3)


def init_schema(cur):
    cur.execute(
        """
        CREATE TABLE IF NOT EXISTS orders (
            id         SERIAL PRIMARY KEY,
            customer   TEXT NOT NULL,
            amount     NUMERIC(10, 2) NOT NULL,
            status     TEXT NOT NULL,
            created_at TIMESTAMPTZ NOT NULL DEFAULT now()
        )
        """
    )
    cur.execute("CREATE INDEX IF NOT EXISTS idx_orders_status ON orders (status)")


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


def do_heavy_select(cur):
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
    (do_insert, 50),
    (do_update, 20),
    (do_delete, 8),
    (do_select, 17),
    (do_heavy_select, 5),
]


def main():
    funcs = [a for a, _ in ACTIONS]
    weights = [w for _, w in ACTIONS]
    conn = connect()
    cur = conn.cursor()
    init_schema(cur)

    step = 0
    while True:
        try:
            random.choices(funcs, weights)[0](cur)
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
            conn = connect()
            cur = conn.cursor()
            init_schema(cur)


if __name__ == "__main__":
    main()
