//== When writing the program Postman was used for testing instead of unit tests. ===

import mongoose, { mongo } from 'mongoose';
import { MongoMemoryServer } from 'mongodb-memory-server';
import request from 'supertest';
import express from 'express';
import Product from '../models/product.model.js';
import productRoutes from '../routes/product.route.js';

let mongoServer;
const app = express();

//setting express app for tests
app.use(express.json());
app.use('/api/products', productRoutes);

//settings before all tests
beforeAll(async () => {
    //creating in-memory MongoDB server
    mongoServer = await MongoMemoryServer.create();
    const uri = mongoServer.getUri();

    //connecting to test DB
    await mongoose.connect(uri);
});

afterEach();

afterAll();

//tests for GETting products

//tests for POST

//tests for PUT

//tests for DELETE
