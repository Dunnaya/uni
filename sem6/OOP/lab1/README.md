# Lab 1

Використовуючи технології Servlets, Filters, JDBC створити програму. Як БД використовувати PostgreSQL.  Please use Keycloak, Auth0 (jwt) Frontend – angular,  react, vue.js. Logging – log4j. REST. Ломбок, mapstruct. MVC pattern. GOF patterns. Not more then 5-6 tables. Builders - Gradle, Maven. Web-server: tomcat. CVS: github, gitlab, pulumi, AWS ECS.

Система Інтернет-магазин. Адміністратор здійснює ведення каталогу Товарів. Клієнт робить та оплачує Замовлення на Товари. Адміністратор може занести неплатників до “чорного списку”.

# Вимоги

Servlets, Filters, JDBC, PostgreSQL, JWT (Auth0), Frontend (React), Log4j, REST, Lombok, MapStruct, MVC, GOF patterns, Maven, Tomcat.

## Запуск

### Передумови
- Java 17+, Maven, Node.js 16+
- PostgreSQL `localhost:5432`
- Tomcat 10+

### 1. Database

```sql
CREATE DATABASE lab1;
```

Підключитись до `lab1` і виконати:

```sql
CREATE TABLE users (
    id SERIAL PRIMARY KEY,
    username VARCHAR(50) UNIQUE NOT NULL,
    role VARCHAR(20) NOT NULL,
    is_blacklisted BOOLEAN DEFAULT FALSE
);
CREATE TABLE products (
    id SERIAL PRIMARY KEY,
    name VARCHAR(100) NOT NULL,
    price DECIMAL(10, 2) NOT NULL,
    stock_quantity INT NOT NULL
);
CREATE TABLE orders (
    id SERIAL PRIMARY KEY,
    user_id INT REFERENCES users(id),
    status VARCHAR(20) DEFAULT 'NEW',
    total_price DECIMAL(10, 2)
);
CREATE TABLE order_items (
    id SERIAL PRIMARY KEY,
    order_id INT REFERENCES orders(id),
    product_id INT REFERENCES products(id),
    quantity INT NOT NULL,
    price_at_purchase DECIMAL(10, 2) NOT NULL
);

INSERT INTO users (username, role) VALUES ('admin1', 'ADMIN'), ('client1', 'CLIENT');
INSERT INTO products (name, price, stock_quantity) VALUES ('Apple MacBook Pro', 2500.00, 10), ('Logitech Mouse', 45.50, 50);
```

### 2. Backend

```bash
cd lab1
mvn clean package
```

Скопіювати `target/lab1-1.0-SNAPSHOT.war` в папку `webapps` Tomcat і запустити Tomcat.

### 3. Frontend

```bash
cd lab1/frontend
npm install
npm start
```

Відкриється `http://localhost:3000`.

## API

| Метод | URL | Опис |
|---|---|---|
| GET | `/api/products` | Список товарів |
| POST | `/api/orders` | Створити замовлення |
| PUT | `/api/orders/{id}` | Оплатити замовлення |
| GET | `/api/orders` | Неоплачені замовлення |
| PUT | `/api/admin/blacklist/{id}?action=block` | Заблокувати клієнта |
| PUT | `/api/admin/blacklist/{id}?action=unblock` | Розблокувати клієнта |

Ендпоінти `/api/orders/*` та `/api/admin/*` захищені JWT (Bearer token).