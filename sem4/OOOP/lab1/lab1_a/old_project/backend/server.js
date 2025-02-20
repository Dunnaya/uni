//const express = require('express')
// "type": "module", in .json file to use import/export syntax
import express from 'express';
import dotenv from 'dotenv';
import { connectDB } from './config/db.js';
import Product from './models/product.model.js';

dotenv.config();

const app = express();

// app.get("/products",(req, res) => {
//     res.send("Server is ready");
// });

app.use(express.json()); //allows to accept JSON data in the req.body

app.post("/api/products", async (req, res) => {
    const product = req.body; // user will send the data

    const newProduct = new Product(product);

    if (!product.name || !product.price || !product.image) {
        return res.status(400).json({ success: false, message: "Please provide all fields" });
    }

    try {
        await newProduct.save();
        res.status(201).json({success: true, data: newProduct});
    }
    catch (error) {
        console.error("Error in creating product", error.message);
        res.status(500).json({success: false, message: "Server Error"});
    }
});

//Postman app for testing APIs
//http://localhost:3000/api/products
//POST request
//Body -> raw -> JSON

//console.log(process.env.MONGO_URI);

app.listen(3000, () => {
    connectDB();
    console.log("Server started at http://localhost:3000");
});