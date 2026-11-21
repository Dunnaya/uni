# Моніторинг PostgreSQL

Інформаційна система моніторингу роботи СУБД: два мікросервіси на Python, PostgreSQL, RabbitMQ, Prometheus, Grafana, Loki і Tempo. Все запускається в Docker. Докладніше про архітектуру: [docs/ARCHITECTURE.md](docs/ARCHITECTURE.md).

```
Користувач ─ login ─► auth-service ◄─ /verify ─ db-service ─ SQL ─► PostgreSQL
                         │ (SQLite)                 ▲                    ▲
                         └── події user.* ─► RabbitMQ ┘          postgres_exporter

Метрики: auth-service, db-service, postgres_exporter ─► Prometheus ─► Grafana
Логи:    контейнери ─► Alloy ─► Loki ─► Grafana
Трейси:  auth-service, db-service ─► Tempo ─► Grafana
```

## Запуск

Потрібен Docker Desktop.

```bash
docker compose up -d --build
docker compose ps    # 10 контейнерів Up, rabbitmq стартує до хвилини
```

| Сервіс | Адреса | Логін |
|--------|--------|-------|
| Grafana, дашборд «PostgreSQL Overview» | http://localhost:3000 | admin / admin12345 |
| Prometheus | http://localhost:9090/targets | - |
| RabbitMQ | http://localhost:15672 | guest / guest |
| auth-service (Swagger) | http://localhost:8001/docs | - |
| db-service (Swagger) | http://localhost:8000/docs | - |

## Перевірка

```bash
# без токена: 401
curl -i http://localhost:8000/orders

# токен (admin створюється при першому старті)
TOKEN=$(curl -s -X POST http://localhost:8001/login \
  -H 'Content-Type: application/json' \
  -d '{"username":"admin","password":"admin12345"}' \
  | python3 -c 'import sys,json; print(json.load(sys.stdin)["access_token"])')

# з токеном: 200
curl -H "Authorization: Bearer $TOKEN" "http://localhost:8000/orders?limit=3"

# події, що пройшли через RabbitMQ
curl -H "Authorization: Bearer $TOKEN" "http://localhost:8000/audit?limit=5"
```

- Логи: Grafana → Explore → Loki, запит `{service="auth-service"}`.
- Трейси: Grafana → Explore → Tempo, Service Name = `auth-service`, або таблиця трейсів внизу дашборда. Перші трейси з'являються через 10-20 секунд.
- Якщо зупинити RabbitMQ (`docker compose stop rabbitmq`), логін усе одно працює, але події за цей час втрачаються.

## Команди

```bash
docker compose logs -f db-service
docker compose down       # зупинити, дані залишаються
docker compose down -v    # зупинити і видалити дані
```

## Структура

```
docker-compose.yml
auth-service/   авторизація: FastAPI, SQLite, JWT
db-service/     навантаження на БД, API, метрики, споживач RabbitMQ
prometheus/  loki/  alloy/  tempo/  grafana/   конфігурації
docs/           архітектура
```

Паролі та `JWT_SECRET` у `docker-compose.yml` навчальні.
