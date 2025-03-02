//const express = require('express')
// "type": "module", in .json file to use import/export syntax
import express from 'express';
import dotenv from 'dotenv';
import { connectDB } from './config/db.js';
import Product from './models/product.model.js';
import mongoose, { mongo } from 'mongoose';

dotenv.config();

const app = express();

// app.get("/products",(req, res) => {
//     res.send("Server is ready");
// });

app.use(express.json()); //allows to accept JSON data in the req.body

app.get("/api/products", async (req, res) => {
    try {
        const products = await Product.find({});
        res.status(200).json({success: true, data: products});
    } catch (error) {
        console.log("Error in fetching products", error.message);
        res.status(500).json({success: false, message: "Server Error"});
    }
});
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

app.put("/api/products/:id", async (req, res) => {
    const {id} = req.params;

    const product = req.body;

    if(!mongoose.Types.ObjectId.isValid(id)) {
        return res.status(404).json({success: false, message: "Invalid Product ID"});
    }

    try {
        const updatedProduct = await Product.findByIdAndUpdate(id, product, {new: true});
        res.status(200).json({success: true, data: updatedProduct});
    } catch (error) {
        res.status(500).json({success: false, message: "Server Error"});
    }
});

app.delete("/api/products/:id", async (req, res) => {
    const {id} = req.params;
    // console.log("id:", id);
    try {
        await Product.findByIdAndDelete(id);
        res.status(200).json({success: true, message: "Product deleted"});
    } catch (error) {
        console.log("Error in deleting product", error.message);
        res.status(404).json({success: false, message: "Product not found"});
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