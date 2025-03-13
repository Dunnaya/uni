import mongoose from "mongoose";
import { MongoMemoryServer } from "mongodb-memory-server";
import request from "supertest";
import express from "express";
import bcrypt from "bcryptjs";
import jwt from "jsonwebtoken";
import dotenv from "dotenv";

import User from "../models/user.model.js";
import userRoutes from "../routes/user.route.js";
import productRoutes from "../routes/product.route.js";

dotenv.config();

let mongoServer;
const app = express();

// Настройка Express для тестов
app.use(express.json());
app.use("/api/users", userRoutes);
app.use("/api/products", productRoutes);

// Запуск in-memory MongoDB перед всеми тестами
beforeAll(async () => {
    mongoServer = await MongoMemoryServer.create();
    const uri = mongoServer.getUri();
    await mongoose.connect(uri);
});

// Очистка базы после каждого теста
afterEach(async () => {
    await User.deleteMany({});
});

// Отключение базы после всех тестов
afterAll(async () => {
    await mongoose.disconnect();
    await mongoServer.stop();
});

describe("User Authentication & Authorization", () => {
    // Тест регистрации пользователя
    test("Should register a new user", async () => {
        const res = await request(app)
            .post("/api/users/register")
            .send({
                name: "Test User",
                email: "test@example.com",
                password: "password123",
            });

        expect(res.statusCode).toBe(201);
        expect(res.body.success).toBe(true);
        expect(res.body.token).toBeDefined();
        expect(res.body.user.role).toBe("user");
    });

    // Тест логина пользователя
    test("Should login an existing user", async () => {
        // Создаем тестового пользователя
        const salt = await bcrypt.genSalt(10);
        const hashedPassword = await bcrypt.hash("password123", salt);

        await User.create({
            name: "Test User",
            email: "test@example.com",
            password: hashedPassword,
            role: "user",
        });

        // Попытка входа
        const res = await request(app)
            .post("/api/users/login")
            .send({
                email: "test@example.com",
                password: "password123",
            });

        expect(res.statusCode).toBe(200);
        expect(res.body.success).toBe(true);
        expect(res.body.token).toBeDefined();
    });

    // Тест запрета создания товара обычным пользователем
    test("Regular user should not be able to create products", async () => {
        // Создаем обычного пользователя
        const salt = await bcrypt.genSalt(10);
        const hashedPassword = await bcrypt.hash("password123", salt);

        const user = await User.create({
            name: "Regular User",
            email: "regular@example.com",
            password: hashedPassword,
            role: "user",
        });

        // Генерируем токен вручную
        const token = jwt.sign({ id: user._id, role: user.role }, process.env.JWT_SECRET, {
            expiresIn: "1h",
        });

        // Пробуем создать товар с токеном обычного пользователя
        const res = await request(app)
            .post("/api/products")
            .set("Authorization", `Bearer ${token}`)
            .send({
                name: "Test Product",
                price: 99.99,
                image: "product.jpg",
            });

        expect(res.statusCode).toBe(403);
        expect(res.body.success).toBe(false);
    });

    // Тест разрешения создания товара админом
    test("Admin user should be able to create products", async () => {
        // Создаем пользователя-администратора
        const salt = await bcrypt.genSalt(10);
        const hashedPassword = await bcrypt.hash("password123", salt);

        const admin = await User.create({
            name: "Admin User",
            email: "admin@example.com",
            password: hashedPassword,
            role: "admin",
        });

        // Генерируем токен вручную
        const token = jwt.sign({ id: admin._id, role: admin.role }, process.env.JWT_SECRET, {
            expiresIn: "1h",
        });

        // Пробуем создать товар с токеном админа
        const res = await request(app)
            .post("/api/products")
            .set("Authorization", `Bearer ${token}`)
            .send({
                name: "Test Product",
                price: 99.99,
                image: "product.jpg",
            });

        expect(res.statusCode).toBe(201);
        expect(res.body.success).toBe(true);
        expect(res.body.data.name).toBe("Test Product");
    });
});
