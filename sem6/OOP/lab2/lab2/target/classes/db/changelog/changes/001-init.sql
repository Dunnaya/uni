--liquibase formatted sql

--changeset dev:001-init
CREATE TABLE IF NOT EXISTS users (
    id              BIGSERIAL PRIMARY KEY,
    username        VARCHAR(100) NOT NULL UNIQUE,
    password        VARCHAR(255) NOT NULL,
    role            VARCHAR(20)  NOT NULL DEFAULT 'USER',
    is_blacklisted  BOOLEAN      NOT NULL DEFAULT FALSE
);

CREATE TABLE IF NOT EXISTS products (
    id             BIGSERIAL PRIMARY KEY,
    name           VARCHAR(200) NOT NULL,
    price          NUMERIC(10,2) NOT NULL,
    stock_quantity INT          NOT NULL DEFAULT 0
);

CREATE TABLE IF NOT EXISTS orders (
    id          BIGSERIAL PRIMARY KEY,
    user_id     BIGINT        NOT NULL REFERENCES users(id),
    status      VARCHAR(20)   NOT NULL DEFAULT 'NEW',
    total_price NUMERIC(10,2) NOT NULL DEFAULT 0
);

CREATE TABLE IF NOT EXISTS order_items (
    id                BIGSERIAL PRIMARY KEY,
    order_id          BIGINT        NOT NULL REFERENCES orders(id),
    product_id        BIGINT        NOT NULL REFERENCES products(id),
    quantity          INT           NOT NULL,
    price_at_purchase NUMERIC(10,2) NOT NULL
);

--rollback DROP TABLE order_items; DROP TABLE orders; DROP TABLE products; DROP TABLE users;
