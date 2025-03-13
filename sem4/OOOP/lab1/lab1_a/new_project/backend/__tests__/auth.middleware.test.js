import mongoose from "mongoose";
import { MongoMemoryServer } from "mongodb-memory-server";
import request from "supertest";
import express from "express";
import bcrypt from "bcryptjs";
import jwt from "jsonwebtoken";
import dotenv from "dotenv";

import User from "../models/user.model.js";
import { protect, authorize } from "../middleware/auth.middleware.js";
import { 
    createProduct, 
    deleteProduct, 
    getProducts, 
    updateProduct 
} from "../controllers/product.controller.js";

dotenv.config();

// creating JWT if not found
if (!process.env.JWT_SECRET) {
    process.env.JWT_SECRET = "test-jwt-secret-for-unit-tests";
}

if (!process.env.JWT_EXPIRE) {
    process.env.JWT_EXPIRE = "1h";
}

let mongoServer;
const app = express();

app.use(express.json());

// сreate custom login controller for test
const loginUser = async (req, res) => {
    try {
        const { email, password } = req.body;

        // validate email & password
        if (!email || !password) {
            return res.status(400).json({
                success: false,
                message: "Please provide an email and password"
            });
        }

        // сheck for user - IMPORTANT: explicitly select password field
        const user = await User.findOne({ email }).select('+password');

        if (!user) {
            return res.status(401).json({
                success: false,
                message: "Invalid credentials"
            });
        }

        // сheck if password matches (!)
        const isMatch = await user.matchPassword(password);

        if (!isMatch) {
            return res.status(401).json({
                success: false,
                message: "Invalid credentials"
            });
        }

        const token = user.getSignedJwtToken();

        res.status(200).json({
            success: true,
            token,
            user: {
                id: user._id,
                name: user.name,
                email: user.email,
                role: user.role
            }
        });
    } catch (error) {
        console.error("Login error:", error);
        res.status(500).json({
            success: false,
            message: error.message
        });
    }
};

// create custom register controller
const registerUser = async (req, res) => {
    try {
        const { name, email, password } = req.body;

        const user = await User.create({
            name,
            email,
            password
        });

        const token = user.getSignedJwtToken();

        res.status(201).json({
            success: true,
            token,
            user: {
                id: user._id,
                name: user.name,
                email: user.email,
                role: user.role
            }
        });
    } catch (error) {
        res.status(400).json({
            success: false,
            message: error.message
        });
    }
};

// simple getMe controller for test
const getMe = async (req, res) => {
    try {
        const user = await User.findById(req.user.id);

        res.status(200).json({
            success: true,
            data: user
        });
    } catch (error) {
        res.status(500).json({
            success: false,
            message: error.message
        });
    }
};

// users routes
app.post("/api/users/register", registerUser);
app.post("/api/users/login", loginUser);
app.get("/api/users/me", protect, getMe);

// products routes
app.get("/api/products", getProducts);
app.post("/api/products", protect, authorize("admin"), createProduct);
app.put("/api/products/:id", protect, authorize("admin"), updateProduct);
app.delete("/api/products/:id", protect, authorize("admin"), deleteProduct);

beforeAll(async () => {
    mongoServer = await MongoMemoryServer.create();
    const uri = mongoServer.getUri();
    await mongoose.connect(uri);
});

afterEach(async () => {
    await User.deleteMany({});
});

afterAll(async () => {
    await mongoose.disconnect();
    await mongoServer.stop();
});

describe("User Authentication & Authorization", () => {
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

    test("Should login an existing user", async () => {
        // directly create a user with a password we know
        const user = new User({
            name: "Test User",
            email: "test@example.com",
            password: "password123",
            role: "user",
        });
        
        // save the user - this will trigger the pre-save hook to hash the password
        await user.save();

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

    test("Regular user should not be able to create products", async () => {
        const salt = await bcrypt.genSalt(10);
        const hashedPassword = await bcrypt.hash("password123", salt);

        const user = await User.create({
            name: "Regular User",
            email: "regular@example.com",
            password: hashedPassword,
            role: "user",
        });

        const token = jwt.sign({ id: user._id, role: user.role }, process.env.JWT_SECRET, {
            expiresIn: "1h",
        });

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

    test("Admin user should be able to create products", async () => {
        // creating admin
        const salt = await bcrypt.genSalt(10);
        const hashedPassword = await bcrypt.hash("password123", salt);

        const admin = await User.create({
            name: "Admin User",
            email: "admin@example.com",
            password: hashedPassword,
            role: "admin",
        });

        // gen admin token
        const token = jwt.sign({ id: admin._id, role: admin.role }, process.env.JWT_SECRET, {
            expiresIn: "1h",
        });

        // admin
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