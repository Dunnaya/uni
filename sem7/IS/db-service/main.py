import logging
import os
import threading
from contextlib import asynccontextmanager, contextmanager
from datetime import datetime
from decimal import Decimal

import httpx
from fastapi import Depends, FastAPI, HTTPException, Query, Response
from fastapi.security import HTTPAuthorizationCredentials, HTTPBearer
from prometheus_client import CONTENT_TYPE_LATEST, generate_latest
from pydantic import BaseModel, Field
from psycopg2.pool import ThreadedConnectionPool

import consumer
import db
import load
import tracing
from metrics import observe

tracing.setup("db-service")  # до logging, щоб у логах був trace_id
logging.basicConfig(level=logging.INFO, format=tracing.LOG_FORMAT)
# pika забагато пише при збоях, приглушуємо
logging.getLogger("pika").setLevel(logging.CRITICAL)
log = logging.getLogger("db-service")

AUTH_URL = os.getenv("AUTH_URL", "http://auth-service:8001")

stop_event = threading.Event()
pool: ThreadedConnectionPool | None = None


@asynccontextmanager
async def lifespan(app: FastAPI):
    global pool
    conn = db.connect(traced=False)
    db.init_schema(conn.cursor())
    conn.close()
    pool = ThreadedConnectionPool(1, 5, **db.DB)
    threading.Thread(target=load.run_load_loop, args=(stop_event,), daemon=True).start()
    threading.Thread(target=consumer.run_consumer, args=(stop_event,), daemon=True).start()
    yield
    stop_event.set()
    pool.closeall()


app = FastAPI(
    title="DB Service",
    description="Робота з PostgreSQL. Ендпоінти /orders потребують JWT від auth-service.",
    version="1.0.0",
    lifespan=lifespan,
)
tracing.instrument_app(app)
bearer = HTTPBearer(auto_error=False)


def require_user(creds: HTTPAuthorizationCredentials | None = Depends(bearer)) -> str:
    if creds is None:
        raise HTTPException(401, "Відсутній токен", headers={"WWW-Authenticate": "Bearer"})
    try:
        resp = httpx.get(
            f"{AUTH_URL}/verify",
            headers={"Authorization": f"Bearer {creds.credentials}"},
            timeout=3,
        )
    except httpx.HTTPError:
        raise HTTPException(503, "Сервіс авторизації недоступний")
    if resp.status_code != 200:
        raise HTTPException(401, "Недійсний або прострочений токен")
    return resp.json()["username"]


@contextmanager
def cursor():
    conn = pool.getconn()
    try:
        conn.autocommit = True
        with conn.cursor() as cur:
            yield cur
    finally:
        pool.putconn(conn)


class OrderIn(BaseModel):
    customer: str = Field(min_length=1, max_length=100)
    amount: Decimal = Field(gt=0, max_digits=10, decimal_places=2)


class Order(BaseModel):
    id: int
    customer: str
    amount: Decimal
    status: str
    created_at: datetime


class AuditEvent(BaseModel):
    id: int
    event_id: str
    event_type: str
    username: str
    occurred_at: datetime
    received_at: datetime


COLUMNS = "id, customer, amount, status, created_at"


@app.get("/health", tags=["service"])
def health():
    return {"status": "ok"}


@app.get("/orders", response_model=list[Order], tags=["orders"])
def list_orders(limit: int = Query(10, ge=1, le=100), user: str = Depends(require_user)):
    """Останні замовлення. Потрібен Bearer-токен."""
    with observe("api_select"), cursor() as cur:
        cur.execute(f"SELECT {COLUMNS} FROM orders ORDER BY id DESC LIMIT %s", (limit,))
        return [Order(**dict(zip(("id", "customer", "amount", "status", "created_at"), r)))
                for r in cur.fetchall()]


@app.post("/orders", response_model=Order, status_code=201, tags=["orders"])
def create_order(body: OrderIn, user: str = Depends(require_user)):
    """Створює замовлення зі статусом new. Потрібен Bearer-токен."""
    with observe("api_insert"), cursor() as cur:
        cur.execute(
            f"INSERT INTO orders (customer, amount, status) VALUES (%s, %s, 'new') RETURNING {COLUMNS}",
            (body.customer, body.amount),
        )
        row = cur.fetchone()
    return Order(**dict(zip(("id", "customer", "amount", "status", "created_at"), row)))


@app.get("/audit", response_model=list[AuditEvent], tags=["events"])
def list_audit(limit: int = Query(20, ge=1, le=200), user: str = Depends(require_user)):
    """Події auth-service (реєстрація, вхід), отримані через RabbitMQ. Потрібен Bearer-токен."""
    with observe("api_audit"), cursor() as cur:
        cur.execute(
            "SELECT id, event_id::text, event_type, username, occurred_at, received_at "
            "FROM audit_events ORDER BY id DESC LIMIT %s",
            (limit,),
        )
        keys = ("id", "event_id", "event_type", "username", "occurred_at", "received_at")
        return [AuditEvent(**dict(zip(keys, r))) for r in cur.fetchall()]


@app.get("/metrics", include_in_schema=False)
def metrics():
    return Response(generate_latest(), media_type=CONTENT_TYPE_LATEST)
