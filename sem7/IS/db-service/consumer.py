import json
import logging
import os
import threading
import time

import pika
import psycopg2
from opentelemetry import propagate, trace
from opentelemetry.trace import SpanKind
from prometheus_client import Counter

import db

log = logging.getLogger("consumer")
tracer = trace.get_tracer("db-service.consumer")

RABBITMQ_URL = os.getenv("RABBITMQ_URL", "")
EXCHANGE = "auth.events"
QUEUE = "db-service.audit"
BINDING_KEY = "user.*"

CONSUMED = Counter(
    "app_events_consumed_total", "Події, отримані з RabbitMQ і збережені", ["event"]
)
CONSUME_ERRORS = Counter(
    "app_events_failed_total", "Події, які не вдалося обробити", ["reason"]
)
for _event in ("user.registered", "user.login", "user.login_failed"):
    CONSUMED.labels(_event)
for _reason in ("bad_message", "db_error", "connection"):
    CONSUME_ERRORS.labels(_reason)


def save_event(cur, event: dict) -> None:
    cur.execute(
        """
        INSERT INTO audit_events (event_id, event_type, username, occurred_at)
        VALUES (%s, %s, %s, %s)
        ON CONFLICT (event_id) DO NOTHING
        """,
        (event["event_id"], event["type"], event["username"], event["timestamp"]),
    )


def handle_message(cur, body: bytes) -> bool:
    try:
        event = json.loads(body)
        event_type = event["type"]
        event["event_id"], event["username"], event["timestamp"]
    except (ValueError, KeyError, TypeError):
        CONSUME_ERRORS.labels("bad_message").inc()
        log.warning("Некоректне повідомлення відкинуто: %r", body[:200])
        return True  # повторна доставка не допоможе
    try:
        save_event(cur, event)
    except psycopg2.DataError:
        CONSUME_ERRORS.labels("bad_message").inc()
        log.warning("Подію з некоректними даними відкинуто: %r", event)
        return True
    # решта psycopg2.Error йде вище: nack і перепідключення
    CONSUMED.labels(event_type).inc()
    return True


def run_consumer(stop: threading.Event) -> None:
    if not RABBITMQ_URL:
        log.info("RABBITMQ_URL не задано, споживач вимкнено")
        return
    while not stop.is_set():
        conn = None
        pg = None
        try:
            pg = db.connect()
            cur = pg.cursor()
            params = pika.URLParameters(RABBITMQ_URL)
            conn = pika.BlockingConnection(params)
            ch = conn.channel()
            ch.exchange_declare(exchange=EXCHANGE, exchange_type="topic", durable=True)
            ch.queue_declare(queue=QUEUE, durable=True)
            ch.queue_bind(queue=QUEUE, exchange=EXCHANGE, routing_key=BINDING_KEY)
            ch.basic_qos(prefetch_count=10)
            log.info("Споживач підключено до RabbitMQ, черга %s", QUEUE)

            for method, props, body in ch.consume(QUEUE, inactivity_timeout=1):
                if stop.is_set():
                    break
                if method is None:
                    continue
                # продовжуємо трейс з заголовків повідомлення
                parent = propagate.extract(props.headers or {})
                with tracer.start_as_current_span(
                    f"{QUEUE} process",
                    context=parent,
                    kind=SpanKind.CONSUMER,
                    attributes={
                        "messaging.system": "rabbitmq",
                        "messaging.destination.name": QUEUE,
                        "messaging.rabbitmq.destination.routing_key": method.routing_key,
                        "messaging.operation": "process",
                    },
                ):
                    try:
                        ok = handle_message(cur, body)
                    except psycopg2.Error as exc:
                        CONSUME_ERRORS.labels("db_error").inc()
                        log.error("Помилка БД при збереженні події: %s", str(exc).strip())
                        ch.basic_nack(method.delivery_tag, requeue=True)
                        raise
                    if ok:
                        ch.basic_ack(method.delivery_tag)
        except Exception as exc:
            CONSUME_ERRORS.labels("connection").inc()
            log.warning("Споживач: %s. Повтор через 5 с", exc)
            stop.wait(5)
        finally:
            for c in (conn, pg):
                try:
                    if c is not None:
                        c.close()
                except Exception:
                    pass
