const express = require('express');
const cors = require('cors');
const helmet = require('helmet');
const morgan = require('morgan');
const path = require('path');

const authRoutes = require('./routes/auth');
const subscriptionRoutes = require('./routes/subscriptions');
const monobankRoutes = require('./routes/monobank');
const importRoutes = require('./routes/import');
const forecastRoutes = require('./routes/forecast');
const errorHandler = require('./middleware/errorHandler');

const app = express();

app.use(helmet({ contentSecurityPolicy: false })); // CSP off - google fonts won't load otherwise
app.use(cors({ origin: process.env.CLIENT_URL || '*' }));
app.use(morgan('dev'));
app.use(express.json());

app.use(express.static(path.join(__dirname, '../public')));

app.use('/api/auth', authRoutes);
app.use('/api/subscriptions', subscriptionRoutes);
app.use('/api/monobank', monobankRoutes);
app.use('/api/import', importRoutes);
app.use('/api/forecast', forecastRoutes);

app.get('/api/health', (req, res) => res.json({ status: 'ok' }));

// JSON 404 for unmatched /api/* routes (must be before SPA fallback)
app.all('/api/*', (req, res) => {
  res.status(404).json({ error: 'Not found' });
});

// SPA fallback for all non-API routes
app.get('*', (req, res) => {
  res.sendFile(path.join(__dirname, '../public/index.html'));
});

app.use(errorHandler);

module.exports = app;