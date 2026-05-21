# Subscription Tracker

Web system for tracking, analyzing and forecasting digital subscription expenses.
Connects to Monobank API, imports CSV bank exports, detects recurring charges automatically, and sends Telegram reminders before billing dates

Built with Node.js + Express + MongoDB

## Features

- manual subscription management (add / edit / pause(resume) / delete)
- Monobank API integration — fetches transactions and auto-detects subscriptions (I hope)
- CSV / XLSX import (monobank_csv and privetbank/otherbank formats)
- billing forecast for 1–24 months ahead
- Telegram bot with reminders N days before each charge
- JWT auth, AES-256 encryption for the Monobank token, rate limiting on login

## Stack

Node.js · Express · MongoDB (Mongoose) · Telegraf · Monobank API · node-cron

## Setup

```bash
git clone <url>
cd subscription-tracker
npm install
cp .env.example .env
# fill in .env
npm run dev
```

Open `http://localhost:3000`

### Environment variables

| Variable | Required | Description |
|----------|----------|-------------|
| `MONGODB_URI` | ✓ | MongoDB connection string |
| `JWT_SECRET` | ✓ | any long random string |
| `CRYPTO_SECRET` | ✓ | min 32 characters (for AES-256) |
| `TELEGRAM_BOT_TOKEN` | – | from @BotFather, bot won't start without it |
| `PORT` | – | default 3000 |

## API

All routes except `/api/auth/*` require `Authorization: Bearer <token>`.

```
POST   /api/auth/register
POST   /api/auth/login          rate limited: 10 req / 15 min per IP
GET    /api/auth/me
POST   /api/auth/link-token     generates Telegram linking token (15 min expiry)

GET    /api/subscriptions       ?active=true&category=entertainment
POST   /api/subscriptions
PUT    /api/subscriptions/:id
DELETE /api/subscriptions/:id
PATCH  /api/subscriptions/:id/toggle

GET    /api/forecast?months=3   1–24 months

POST   /api/monobank/token      save + verify token
DELETE /api/monobank/token
POST   /api/monobank/sync
GET    /api/monobank/status

POST   /api/import              multipart/form-data: file + bank (monobank_csv | privatbank | generic)
```

## Telegram bot

Commands: `/subscriptions`, `/upcoming`

To link: Settings → generate token → send `/start <token>` to the bot
Reminders run at 09:00 daily, default 3 days before billing date (not curr possible to change tbh)