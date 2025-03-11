//== When writing the program Postman was used for testing instead of unit tests. ===

import mongoose from 'mongoose';
import { MongoMemoryServer } from 'mongodb-memory-server';
import request from 'supertest';
import express from 'express';
import Product from '../models/product.model.js';
import productRoutes from '../routes/product.route.js';

let mongoServer;
const app = express();

//setting Express app for tests
app.use(express.json());
app.use('/api/products', productRoutes);

//DB settings before all tests
beforeAll(async () => {
    //creating in-memory MongoDB server
    mongoServer = await MongoMemoryServer.create();
    const uri = mongoServer.getUri();

    //connecting to test DB
    await mongoose.connect(uri);
});

//cleaning after each test
afterEach(async () => {
    await Product.deleteMany({});
});

//disconnect and close after all tests
afterAll(async () => {
    await mongoose.disconnect();
    await mongoServer.stop();
});

//tests for GETting products
describe('GET /api/products', () => {
    
});

//tests for POST
describe('POST /api/products', () => {
    it('must return an empty arr if no products', async () => {
        const res = await request(app).get('/api/products');

        expect(res.statusCode).toBe(200);
        expect(res.body.success).toBe(true);
        expect(res.body.data).toBeInstanceOf(Array);
        expect(res.body.data.length).toBe(0);
    });

    it('must return the products array', async () => {
        //creating test product
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

//tests for PUT
describe('PUT /api/products', () => {
    
});

//tests for DELETE
describe('DELETE /api/products', () => {
    
});