# Lab 2

Використовуючи технології Spring, Hibernate, Spring Data, Spring Security, Liquibase, Keycloak, Auth0(jwt).

Система Інтернет-магазин. Адміністратор здійснює ведення каталогу Товарів. Клієнт робить та оплачує Замовлення на Товари. Адміністратор може занести неплатників до “чорного списку”.

# Вимоги

Spring Boot, Spring MVC, Spring Security, Spring Data JPA, Hibernate, Liquibase, PostgreSQL, JWT (Auth0), Frontend (React), Lombok, MapStruct, MVC, GOF patterns, Maven.

## Запуск

### Передумови
- Java 17+, Maven, Node.js 16+
- PostgreSQL `localhost:5432`

### 1. Database

```sql
CREATE DATABASE lab2;
```

Liquibase автоматично створює таблиці та наповнює їх тестовими даними при першому запуску.

### 2. Backend

```bash
cd lab2
mvn spring-boot:run
```

### 3. Frontend

```bash
cd lab2/frontend
npm install
npm start
```

Відкриється `http://localhost:3000`.

Test accounts: `admin / password` | `client1 / password`

## API

| Method | URL | Description |
|---|---|---|
| POST | `/api/auth/login` | Login, returns JWT token |
| GET | `/api/products` | Product list (public) |
| POST | `/api/orders` | Create order (JWT) |
| PUT | `/api/orders/{id}` | Pay order (JWT) |
| GET | `/api/orders` | Unpaid orders (JWT, ADMIN) |
| PUT | `/api/admin/blacklist/{id}?action=block` | Block user (JWT, ADMIN) |
| PUT | `/api/admin/blacklist/{id}?action=unblock` | Unblock user (JWT, ADMIN) |

Endpoints `/api/orders/*` and `/api/admin/*` are protected by JWT (Bearer token).