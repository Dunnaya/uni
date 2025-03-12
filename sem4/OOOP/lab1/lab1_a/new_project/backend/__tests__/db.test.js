import mongoose, { connection } from "mongoose";
import { connectDB } from '../config/db.js'

//... console methods
console.log = jest.fn();
console.error = jest.fn();
process.exit = jest.fn();

describe('Tests for connecting to DB', () => {
    beforeEach(() => {
        //reseting all mocks before each test
        jest.clearAllMocks();
    });

    afterEach(async () => {
        //closing connection after each test
        await mongoose.connection.close();
    });

    it('must connect to DB successfully', async () => {
        mongoose.connect = jest.fn().mockResolvedValue({
            connection: {
                host: 'test-host'
            }
        });

        process.env.MONGO_URI = 'mongodb://test-uri';

        await connectDB();

        expect(mongoose.connect).toHaveBeenCalledWith('mongodb://test-uri');
        expect(console.log).toHaveBeenCalledWith('MongoDB Connected: test-host');
        expect(process.exit).not.toHaveBeenCalled();
    });

    it('must stop the process when error appears', async () => {
        mongoose.connect = jest.fn().mockRejectedValue(new Error('Connection failed'));
        await connectDB();

        expect(console.error).toHaveBeenCalledWith('Error: Connection failed');
        expect(process.exit).toHaveBeenCalledWith(1);
    });
});