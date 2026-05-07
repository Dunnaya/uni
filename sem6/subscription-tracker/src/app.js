const express = require('express');
const cors = require('cors');
const helmet = require('helmet');
const morgan = require('morgan');
const multer = require('multer');

const authRoutes = require('./routes/auth');
const subscriptionRoutes = require('./routes/subscriptions');
const monobankRoutes = require('./routes/monobank');
const importRoutes = require('./routes/import');
const forecastRoutes = require('./routes/forecast');
const errorHandler = require('./middleware/errorHandler');

const app = express();

// security and utilities
app.use(helmet());
app.use(cors({ origin: process.env.CLIENT_URL || '*' }));
app.use(morgan('dev'));
app.use(express.json());

// routes
app.use('/api/auth', authRoutes);
app.use('/api/subscriptions', subscriptionRoutes);
app.use('/api/monobank', monobankRoutes);
app.use('/api/import', importRoutes);
app.use('/api/forecast', forecastRoutes);

// health-check
app.get('/api/health', (req, res) => res.json({ status: 'ok' }));

// centralized error handling (always last)
app.use(errorHandler);

module.exports = app;