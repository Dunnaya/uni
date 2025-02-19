//const express = require('express')
// "type": "module", in .json file to use import/export syntax
import express from 'express';
import dotenv from 'dotenv';
import { connectDB } from './config/db.js';

dotenv.config();

const app = express();

app.get("/products",(req, res) => {
    res.send("Server is ready");
});

//console.log(process.env.MONGO_URI);

app.listen(3000, () => {
    connectDB();
    console.log("Server started at http://localhost:3000");
});