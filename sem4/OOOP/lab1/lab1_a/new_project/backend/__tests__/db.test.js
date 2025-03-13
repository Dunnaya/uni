import mongoose from "mongoose";
import { connectDB } from '../config/db.js';

const originalConsoleLog = console.log;
const originalConsoleError = console.error;
const originalProcessExit = process.exit;

describe('Tests for connecting to DB', () => {
    beforeEach(() => {
        console.log = jest.fn();
        console.error = jest.fn();
        process.exit = jest.fn();
        
        jest.clearAllMocks();
    });

    afterEach(() => {
        console.log = originalConsoleLog;
        console.error = originalConsoleError;
        process.exit = originalProcessExit;
    });

    it('must connect to DB successfully', async () => {
        const originalConnect = mongoose.connect;
        mongoose.connect = jest.fn().mockResolvedValue({
            connection: {
                host: 'test-host'
            }
        });

        process.env.MONGO_URI = 'mongodb://test-uri';

        try {
            await connectDB();

            expect(mongoose.connect).toHaveBeenCalledWith('mongodb://test-uri');
            expect(console.log).toHaveBeenCalledWith('MongoDB Connected: test-host');
            expect(process.exit).not.toHaveBeenCalled();
        } finally {
            mongoose.connect = originalConnect;
        }
    });

    it('must stop the process when error appears', async () => {
        const originalConnect = mongoose.connect;
        mongoose.connect = jest.fn().mockRejectedValue(new Error('Connection failed'));
        
        try {
            await connectDB();

            expect(console.error).toHaveBeenCalledWith('Error: Connection failed');
            expect(process.exit).toHaveBeenCalledWith(1);
        } finally {
            mongoose.connect = originalConnect;
        }
    });
});