--liquibase formatted sql

--changeset dev:002-seed
-- password = "password" encoded with BCrypt
INSERT INTO users (username, password, role, is_blacklisted) VALUES
    ('admin',   '$2a$10$7EqJtq98hPqEX7fNZaFWoO3vdK5nL2Ee1lSf9S1lI.Rv5CQqA5aFu', 'ADMIN', FALSE),
    ('client1', '$2a$10$7EqJtq98hPqEX7fNZaFWoO3vdK5nL2Ee1lSf9S1lI.Rv5CQqA5aFu', 'USER',  FALSE);

INSERT INTO products (name, price, stock_quantity) VALUES
    ('Laptop',     999.99, 10),
    ('Mouse',       25.50, 50),
    ('Keyboard',    49.99, 30),
    ('Monitor',    299.00, 15);

--rollback DELETE FROM products; DELETE FROM users;