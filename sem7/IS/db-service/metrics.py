import time
from contextlib import contextmanager

from prometheus_client import Counter, Histogram

DB_OPS = Counter(
    "app_db_operations_total", "Успішні операції з PostgreSQL", ["operation"]
)
DB_ERRORS = Counter(
    "app_db_errors_total", "Невдалі операції з PostgreSQL", ["operation"]
)
DB_LATENCY = Histogram(
    "app_db_operation_seconds", "Тривалість операцій з PostgreSQL", ["operation"]
)


# створюємо серії одразу з нулем, щоб rate() бачив перший приріст
for _op in ("insert", "update", "delete", "select", "aggregate",
            "api_select", "api_insert", "api_audit"):
    DB_OPS.labels(_op)
    DB_ERRORS.labels(_op)
    DB_LATENCY.labels(_op)


@contextmanager
def observe(operation: str):
    start = time.perf_counter()
    try:
        yield
    except Exception:
        DB_ERRORS.labels(operation).inc()
        raise
    finally:
        DB_LATENCY.labels(operation).observe(time.perf_counter() - start)
    DB_OPS.labels(operation).inc()
