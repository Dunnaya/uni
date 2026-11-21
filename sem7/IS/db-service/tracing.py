import os

from opentelemetry import trace
from opentelemetry.exporter.otlp.proto.http.trace_exporter import OTLPSpanExporter
from opentelemetry.instrumentation.fastapi import FastAPIInstrumentor
from opentelemetry.instrumentation.httpx import HTTPXClientInstrumentor
from opentelemetry.instrumentation.logging import LoggingInstrumentor
from opentelemetry.instrumentation.psycopg2 import Psycopg2Instrumentor
from opentelemetry.sdk.resources import Resource
from opentelemetry.sdk.trace import TracerProvider
from opentelemetry.sdk.trace.export import BatchSpanProcessor

ENABLED = bool(os.getenv("OTEL_EXPORTER_OTLP_ENDPOINT"))

# ці адреси не трасуємо
EXCLUDED_URLS = "health,metrics,docs,openapi.json"

LOG_FORMAT = "%(asctime)s %(levelname)s %(name)s [trace_id=%(otelTraceID)s] %(message)s"


def setup(service_name: str) -> None:
    LoggingInstrumentor().instrument(set_logging_format=False)
    if not ENABLED:
        return
    provider = TracerProvider(resource=Resource.create({"service.name": service_name}))
    provider.add_span_processor(BatchSpanProcessor(OTLPSpanExporter()))
    trace.set_tracer_provider(provider)
    HTTPXClientInstrumentor().instrument()
    # з'єднання генератора навантаження не трасується, див. db.connect
    Psycopg2Instrumentor().instrument(skip_dep_check=True)


def instrument_app(app) -> None:
    if ENABLED:
        FastAPIInstrumentor.instrument_app(app, excluded_urls=EXCLUDED_URLS)

