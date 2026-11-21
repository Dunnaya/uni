import contextvars
import hashlib
import logging
import hmac
import os
import secrets
import sqlite3
import threading
import time
from contextlib import asynccontextmanager, closing

import jwt
from fastapi import BackgroundTasks, Depends, FastAPI, HTTPException, Response
from fastapi.security import HTTPAuthorizationCredentials, HTTPBearer
from prometheus_client import CONTENT_TYPE_LATEST, Counter, generate_latest
from opentelemetry import trace
from pydantic import BaseModel, Field

import events
import tracing

tracing.setup("auth-service")  # до logging, щоб у логах був trace_id
logging.basicConfig(level=logging.INFO, format=tracing.LOG_FORMAT)
# pika забагато пише при збоях, приглушуємо
logging.getLogger("pika").setLevel(logging.CRITICAL)
log = logging.getLogger("auth")
tracer = trace.get_tracer("auth-service")

JWT_SECRET = os.getenv("JWT_SECRET", "change-me-in-production")
JWT_ALGORITHM = "HS256"
TOKEN_TTL = int(os.getenv("TOKEN_TTL_SECONDS", "3600"))
DB_PATH = os.getenv("SQLITE_PATH", "./auth.db")
ADMIN_USER = os.getenv("ADMIN_USER", "admin")
ADMIN_PASSWORD = os.getenv("ADMIN_PASSWORD", "admin12345")

LOGINS = Counter("auth_login_total", "Спроби входу", ["result"])
REGISTRATIONS = Counter("auth_register_total", "Зареєстровані користувачі")
VERIFICATIONS = Counter("auth_verify_total", "Перевірки токенів", ["result"])

# створюємо серії одразу з нулем, щоб rate() бачив перший приріст
for _r in ("success", "fail"):
    LOGINS.labels(_r)
for _r in ("ok", "invalid", "missing"):
    VERIFICATIONS.labels(_r)


def db() -> sqlite3.Connection:
    conn = sqlite3.connect(DB_PATH)
    conn.row_factory = sqlite3.Row
    return conn


def hash_password(password: str, salt: bytes | None = None) -> str:
    salt = salt or secrets.token_bytes(16)
    digest = hashlib.pbkdf2_hmac("sha256", password.encode(), salt, 200_000)
    return f"{salt.hex()}${digest.hex()}"


def check_password(password: str, stored: str) -> bool:
    salt_hex, digest_hex = stored.split("$")
    candidate = hash_password(password, bytes.fromhex(salt_hex)).split("$")[1]
    return hmac.compare_digest(candidate, digest_hex)


def init_db() -> None:
    os.makedirs(os.path.dirname(os.path.abspath(DB_PATH)), exist_ok=True)
    with closing(db()) as conn, conn:
        conn.execute(
            """
            CREATE TABLE IF NOT EXISTS users (
                id            INTEGER PRIMARY KEY AUTOINCREMENT,
                username      TEXT UNIQUE NOT NULL,
                password_hash TEXT NOT NULL,
                created_at    TEXT NOT NULL DEFAULT CURRENT_TIMESTAMP
            )
            """
        )
        exists = conn.execute(
            "SELECT 1 FROM users WHERE username = ?", (ADMIN_USER,)
        ).fetchone()
        if not exists:
            conn.execute(
                "INSERT INTO users (username, password_hash) VALUES (?, ?)",
                (ADMIN_USER, hash_password(ADMIN_PASSWORD)),
            )


class RegisterIn(BaseModel):
    username: str = Field(min_length=3, max_length=50)
    password: str = Field(min_length=6, max_length=128)


class LoginIn(BaseModel):
    username: str
    password: str


class TokenOut(BaseModel):
    access_token: str
    token_type: str = "bearer"
    expires_in: int


class VerifyOut(BaseModel):
    valid: bool
    username: str


@asynccontextmanager
async def lifespan(app: FastAPI):
    init_db()
    yield


app = FastAPI(
    title="Auth Service",
    description="Реєстрація, логін та перевірка JWT-токенів.",
    version="1.0.0",
    lifespan=lifespan,
)
tracing.instrument_app(app)
bearer = HTTPBearer(auto_error=False)


def current_user(creds: HTTPAuthorizationCredentials | None = Depends(bearer)) -> str:
    if creds is None:
        VERIFICATIONS.labels("missing").inc()
        raise HTTPException(401, "Відсутній токен", headers={"WWW-Authenticate": "Bearer"})
    try:
        payload = jwt.decode(creds.credentials, JWT_SECRET, algorithms=[JWT_ALGORITHM])
    except jwt.PyJWTError:
        VERIFICATIONS.labels("invalid").inc()
        raise HTTPException(
            401, "Недійсний або прострочений токен", headers={"WWW-Authenticate": "Bearer"}
        )
    VERIFICATIONS.labels("ok").inc()
    return payload["sub"]


@app.get("/health", tags=["service"])
def health():
    return {"status": "ok"}


@app.post("/register", status_code=201, tags=["auth"])
def register(body: RegisterIn, background: BackgroundTasks):
    try:
        with closing(db()) as conn, conn:
            conn.execute(
                "INSERT INTO users (username, password_hash) VALUES (?, ?)",
                (body.username, hash_password(body.password)),
            )
    except sqlite3.IntegrityError:
        raise HTTPException(409, "Користувач уже існує")
    REGISTRATIONS.inc()
    log.info("Зареєстровано користувача: user=%s", body.username)
    background.add_task(events.publish, "user.registered", body.username)
    return {"username": body.username}


@app.post("/login", response_model=TokenOut, tags=["auth"])
def login(body: LoginIn, background: BackgroundTasks):
    with tracer.start_as_current_span(
        "SELECT users",
        attributes={
            "db.system": "sqlite",
            "db.statement": "SELECT password_hash FROM users WHERE username = ?",
        },
    ):
        with closing(db()) as conn:
            row = conn.execute(
                "SELECT password_hash FROM users WHERE username = ?", (body.username,)
            ).fetchone()
    with tracer.start_as_current_span("check_password"):
        password_ok = row is not None and check_password(body.password, row["password_hash"])
    if not password_ok:
        LOGINS.labels("fail").inc()
        log.warning("Невдала спроба входу: user=%s", body.username)
        # при HTTPException BackgroundTasks не спрацює, тому окремий потік
        ctx = contextvars.copy_context()
        threading.Thread(
            target=ctx.run,
            args=(events.publish, "user.login_failed", body.username),
            daemon=True,
        ).start()
        raise HTTPException(401, "Невірний логін або пароль")
    LOGINS.labels("success").inc()
    log.info("Успішний вхід: user=%s", body.username)
    background.add_task(events.publish, "user.login", body.username)
    now = int(time.time())
    token = jwt.encode(
        {"sub": body.username, "iat": now, "exp": now + TOKEN_TTL},
        JWT_SECRET,
        algorithm=JWT_ALGORITHM,
    )
    return TokenOut(access_token=token, expires_in=TOKEN_TTL)


@app.get("/verify", response_model=VerifyOut, tags=["auth"])
def verify(username: str = Depends(current_user)):
    return VerifyOut(valid=True, username=username)


@app.get("/metrics", include_in_schema=False)
def metrics():
    return Response(generate_latest(), media_type=CONTENT_TYPE_LATEST)
