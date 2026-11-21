import logging
import os
import time

import psycopg2

log = logging.getLogger("db")

DB = {
    "host": os.getenv("DB_HOST", "localhost"),
    "port": int(os.getenv("DB_PORT", "5432")),
    "dbname": os.getenv("DB_NAME", "appdb"),
    "user": os.getenv("DB_USER", "app"),
    "password": os.getenv("DB_PASSWORD", "app"),
}


def connect(traced: bool = True):
    while True:
        try:
            conn = psycopg2.connect(**DB)
            if not traced:
                conn.cursor_factory = psycopg2.extensions.cursor
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
    cur.execute(
        """
        CREATE TABLE IF NOT EXISTS audit_events (
            id          SERIAL PRIMARY KEY,
            event_id    UUID UNIQUE NOT NULL,
            event_type  TEXT NOT NULL,
            username    TEXT NOT NULL,
            occurred_at TIMESTAMPTZ NOT NULL,
            received_at TIMESTAMPTZ NOT NULL DEFAULT now()
        )
        """
    )
