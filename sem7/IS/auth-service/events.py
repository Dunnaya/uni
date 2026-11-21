import json
import logging
import os
import uuid
from datetime import datetime, timezone

import pika
from opentelemetry import propagate, trace
from opentelemetry.trace import SpanKind
from prometheus_client import Counter

log = logging.getLogger("events")
tracer = trace.get_tracer("auth-service.events")

RABBITMQ_URL = os.getenv("RABBITMQ_URL", "")
EXCHANGE = "auth.events"

PUBLISHED = Counter(
    "auth_events_published_total", "Події, опубліковані в RabbitMQ", ["event", "result"]
)
for _event in ("user.registered", "user.login", "user.login_failed"):
    for _result in ("ok", "error"):
        PUBLISHED.labels(_event, _result)


def publish(event_type: str, username: str) -> None:
    if not RABBITMQ_URL:
        return
    body = {
        "event_id": str(uuid.uuid4()),
        "type": event_type,
        "username": username,
        "timestamp": datetime.now(timezone.utc).isoformat(),
    }
    try:
        with tracer.start_as_current_span(
            f"{EXCHANGE} publish",
            kind=SpanKind.PRODUCER,
            attributes={
                "messaging.system": "rabbitmq",
                "messaging.destination.name": EXCHANGE,
                "messaging.rabbitmq.destination.routing_key": event_type,
                "messaging.operation": "publish",
            },
        ):
            headers: dict = {}
            propagate.inject(headers)
            params = pika.URLParameters(RABBITMQ_URL)
            params.socket_timeout = 2
            params.connection_attempts = 1
            conn = pika.BlockingConnection(params)
            try:
                ch = conn.channel()
                ch.exchange_declare(exchange=EXCHANGE, exchange_type="topic", durable=True)
                ch.basic_publish(
                    exchange=EXCHANGE,
                    routing_key=event_type,
                    body=json.dumps(body),
                    properties=pika.BasicProperties(
                        content_type="application/json", delivery_mode=2, headers=headers
                    ),
                )
            finally:
                conn.close()
        PUBLISHED.labels(event_type, "ok").inc()
    except Exception as exc:  # брокер не має ламати логін
        PUBLISHED.labels(event_type, "error").inc()
        log.warning("Не вдалося опублікувати %s: %s", event_type, exc)
