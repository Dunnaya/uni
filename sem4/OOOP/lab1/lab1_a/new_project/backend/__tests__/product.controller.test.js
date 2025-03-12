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

//tests for POST
describe('POST /api/products', () => {
    it('must create a new product', async () => {
        const productData = {
            name: 'New product',
            price: 1200,
            image: 'new-product.png'
        };

        const res = await request(app)
        .post('/api/products')
        .send(productData);

        expect(res.statusCode).toBe(201);
        expect(res.body.success).toBe(true);
        expect(res.body.data.name).toBe(productData.name);
        expect(res.body.data.price).toBe(productData.price);

        //is product saved in DB?
        const savedProduct = await Product.findById(res.body.data._id);
        expect(savedProduct).not.toBeNull();
        expect(savedProduct.name).toBe(productData.name);
    });

    it('must return error if not enough data', async () => {
        const invalidProduct = {
            name: 'Not compeled product'
            //there is no price and img
        };

        const res = await request(app)
        .post('/api/products')
        .send(invalidProduct);

        expect(res.statusCode).toBe(400);
        expect(res.body.success).toBe(false);
        expect(res.body.message).toBe('Please provide all fields');

        //this product should not be saved
        const products = await Product.find({});
        expect(products.length).toBe(0);
    });
});

//tests for PUT
describe('PUT /api/products/:id', () => {
    it('must update existing product', async () => {
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

        expect(res.statusCode).toBe(200);
        expect(res.body.success).toBe(true);
        expect(res.body.data.name).toBe(updateData.name);
        expect(res.body.data.price).toBe(updateData.price);

        //DB check
        const updatedProduct = await Product.findById(product._id);
        expect(updatedProduct.name).toBe(updateData.name);
    });

    it('must return error if ID is wrong', async () => {
        const invalidID = 'invalid-id';

        const res = await request(app)
        .put(`/api/products/${invalidID}`)
        .send({name: 'Test', price: 100, image: 'test.png'});

        expect(res.statusCode).toBe(404);
        expect(res.body.success).toBe(false);
        expect(res.body.message).toBe('Invalid Product Id');
    });
});

//tests for DELETE
describe('DELETE /api/products/:id', () => {
    it('must delete existing product', async () => {

        const product = await Product.create({
            name: 'Deleted product',
            price: 500,
            image: 'delete-me.png'
        });

        const res = await request(app)
        .delete(`/api/products/${product._id}`);

        expect(res.statusCode).toBe(200);
        expect(res.body.success).toBe(true);
        expect(res.body.message).toBe('Product deleted');

        //check DB
        const deletedProduct = await Product.findById(product._id);
        expect(deletedProduct).toBeNull();
    });

    it('must return error if ID is wrong', async () => {
        const invalidID = 'invalid-id';

        const res = await request(app)
        .delete(`/api/products/${invalidID}`);

        expect(res.statusCode).toBe(404);
        expect(res.body.success).toBe(false);
        expect(res.body.message).toBe('Invalid Product Id');
    });
});