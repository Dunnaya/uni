import mongoose from 'mongoose';
import { MongoMemoryServer } from 'mongodb-memory-server';
import request from 'supertest';
import express from 'express';
import jwt from 'jsonwebtoken';
import Product from '../models/product.model.js';
import User from '../models/user.model.js';
import { getProducts, createProduct, updateProduct, deleteProduct } from '../controllers/product.controller.js';
import { protect, authorize } from '../middleware/auth.middleware.js';

// Создаем тестовое значение для JWT_SECRET, если оно не определено
if (!process.env.JWT_SECRET) {
    process.env.JWT_SECRET = "test-jwt-secret-for-unit-tests";
}

let mongoServer;
const app = express();
let adminToken;
let admin;

// Настройка Express для тестов
app.use(express.json());

// Настраиваем маршруты так же, как в приложении
app.get('/api/products', getProducts);
app.post('/api/products', protect, authorize("admin"), createProduct);
app.put('/api/products/:id', protect, authorize("admin"), updateProduct);
app.delete('/api/products/:id', protect, authorize("admin"), deleteProduct);

// Настройка БД перед всеми тестами
beforeAll(async () => {
    // Создаем in-memory MongoDB сервер
    mongoServer = await MongoMemoryServer.create();
    const uri = mongoServer.getUri();

    // Подключаемся к тестовой БД
    await mongoose.connect(uri);

    // Создаем пользователя-администратора для тестов
    admin = await User.create({
        name: 'Admin User',
        email: 'admin@example.com',
        password: 'hashedPassword', // В реальности должен быть хэш
        role: 'admin'
    });

    // Генерируем токен администратора
    adminToken = jwt.sign({ id: admin._id, role: admin.role }, process.env.JWT_SECRET, {
        expiresIn: '1h'
    });
});

// Очистка после каждого теста
afterEach(async () => {
    await Product.deleteMany({});
});

// Отключение и закрытие после всех тестов
afterAll(async () => {
    await User.deleteMany({});
    await mongoose.disconnect();
    await mongoServer.stop();
});

// Тесты для GET /api/products
describe('GET /api/products', () => {
    it('must return an empty array if no products', async () => {
        const res = await request(app).get('/api/products');

        expect(res.statusCode).toBe(200);
        expect(res.body.success).toBe(true);
        expect(res.body.data).toBeInstanceOf(Array);
        expect(res.body.data.length).toBe(0);
    });

    it('must return the products array', async () => {
        // Создаем тестовый продукт
        await Product.create({
            name: 'Test product',
            price: 999,
            image: 'test-image.png'
        });

        const res = await request(app).get('/api/products');

        expect(res.statusCode).toBe(200);
        expect(res.body.success).toBe(true);
        expect(res.body.data).toBeInstanceOf(Array);
        expect(res.body.data.length).toBe(1);
        expect(res.body.data[0].name).toBe('Test product');
    });
});

// Тесты для POST /api/products - теперь требуется аутентификация
describe('POST /api/products', () => {
    it('must create a new product when admin is authenticated', async () => {
        const productData = {
            name: 'New product',
            price: 1200,
            image: 'new-product.png'
        };

        const res = await request(app)
        .post('/api/products')
        .set('Authorization', `Bearer ${adminToken}`)
        .send(productData);

        expect(res.statusCode).toBe(201);
        expect(res.body.success).toBe(true);
        expect(res.body.data.name).toBe(productData.name);
        expect(res.body.data.price).toBe(productData.price);

        // Проверяем сохранение в БД
        const savedProduct = await Product.findById(res.body.data._id);
        expect(savedProduct).not.toBeNull();
        expect(savedProduct.name).toBe(productData.name);
    });

    it('must return error if not authenticated', async () => {
        const productData = {
            name: 'New product',
            price: 1200,
            image: 'new-product.png'
        };

        const res = await request(app)
        .post('/api/products')
        .send(productData);

        expect(res.statusCode).toBe(401);
        expect(res.body.success).toBe(false);
    });

    it('must return error if not enough data', async () => {
        const invalidProduct = {
            name: 'Not completed product'
            // Нет цены и изображения
        };

        const res = await request(app)
        .post('/api/products')
        .set('Authorization', `Bearer ${adminToken}`)
        .send(invalidProduct);

        expect(res.statusCode).toBe(400);
        expect(res.body.success).toBe(false);
        expect(res.body.message).toBe('Please provide all fields');

        // Продукт не должен быть сохранен
        const products = await Product.find({});
        expect(products.length).toBe(0);
    });
});

// Тесты для PUT /api/products/:id - теперь требуется аутентификация
describe('PUT /api/products/:id', () => {
    it('must update existing product when admin is authenticated', async () => {
        const product = await Product.create({
            name: 'Old name',
            price: 1000,
            image: 'old-image.png'
        });

        const updateData = {
            name: 'New name',
            price: 1500,
            image: 'new-image.png'
        };

        const res = await request(app)
        .put(`/api/products/${product._id}`)
        .set('Authorization', `Bearer ${adminToken}`)
        .send(updateData);

        expect(res.statusCode).toBe(200);
        expect(res.body.success).toBe(true);
        expect(res.body.data.name).toBe(updateData.name);
        expect(res.body.data.price).toBe(updateData.price);

        // Проверка БД
        const updatedProduct = await Product.findById(product._id);
        expect(updatedProduct.name).toBe(updateData.name);
    });

    it('must return error if not authenticated', async () => {
        const product = await Product.create({
            name: 'Old name',
            price: 1000,
            image: 'old-image.png'
        });

        const updateData = {
            name: 'New name',
            price: 1500,
            image: 'new-image.png'
        };

        const res = await request(app)
        .put(`/api/products/${product._id}`)
        .send(updateData);

        expect(res.statusCode).toBe(401);
        expect(res.body.success).toBe(false);
    });

    it('must return error if ID is wrong', async () => {
        const invalidID = 'invalid-id';

        const res = await request(app)
        .put(`/api/products/${invalidID}`)
        .set('Authorization', `Bearer ${adminToken}`)
        .send({name: 'Test', price: 100, image: 'test.png'});

        expect(res.statusCode).toBe(404);
        expect(res.body.success).toBe(false);
        expect(res.body.message).toBe('Invalid Product Id');
    });
});

// Тесты для DELETE /api/products/:id - теперь требуется аутентификация
describe('DELETE /api/products/:id', () => {
    it('must delete existing product when admin is authenticated', async () => {
        const product = await Product.create({
            name: 'Deleted product',
            price: 500,
            image: 'delete-me.png'
        });

        const res = await request(app)
        .delete(`/api/products/${product._id}`)
        .set('Authorization', `Bearer ${adminToken}`);

        expect(res.statusCode).toBe(200);
        expect(res.body.success).toBe(true);
        expect(res.body.message).toBe('Product deleted');

        // Проверка БД
        const deletedProduct = await Product.findById(product._id);
        expect(deletedProduct).toBeNull();
    });

    it('must return error if not authenticated', async () => {
        const product = await Product.create({
            name: 'Deleted product',
            price: 500,
            image: 'delete-me.png'
        });

        const res = await request(app)
        .delete(`/api/products/${product._id}`);

        expect(res.statusCode).toBe(401);
        expect(res.body.success).toBe(false);
    });

    it('must return error if ID is wrong', async () => {
        const invalidID = 'invalid-id';

        const res = await request(app)
        .delete(`/api/products/${invalidID}`)
        .set('Authorization', `Bearer ${adminToken}`);

        expect(res.statusCode).toBe(404);
        expect(res.body.success).toBe(false);
        expect(res.body.message).toBe('Invalid Product Id');
    });
});