# Subscription Tracker

Web system for tracking, analyzing and forecasting digital subscription expenses.
Connects to Monobank API, imports CSV bank exports, detects recurring charges automatically, and sends Telegram reminders before billing dates.

**Repository:** https://github.com/Dunnaya/subscription-tracker
**Live demo:** https://subscription-tracker-production-0405.up.railway.app

Built with Node.js + Express + MongoDB

## Features

- manual subscription management (add / edit / pause(resume) / delete)
- Monobank API integration — fetches transactions and auto-detects subscriptions (I hope)
- CSV / XLSX import (monobank_csv and privetbank/otherbank formats)
- billing forecast for 1–12 months ahead
- Telegram bot with reminders N days before each charge
- JWT auth, AES-256 encryption for the Monobank token, rate limiting on login

## Stack

Node.js · Express · MongoDB (Mongoose) · Telegraf · Monobank API · node-cron

### Environment variables

| Variable | Required | Description |
|----------|----------|-------------|
| `MONGODB_URI` | ✓ | MongoDB connection string |
| `JWT_SECRET` | ✓ | any long random string |
| `CRYPTO_SECRET` | ✓ | min 32 characters (for AES-256) |
| `TELEGRAM_BOT_TOKEN` | – | from @BotFather, bot won't start without it |
| `PORT` | – | default 3000 |
| `NOTIFICATION_HOUR` | – | hour to send daily reminders (default 9) |
| `NOTIFICATION_MINUTE` | – | minute to send daily reminders (default 0) |
| `TZ` | – | timezone for scheduler (default `Europe/Kyiv`) |
| `CLIENT_URL` | – | allowed CORS origin, default `*` |

## API

All routes except `/api/auth/register`, `/api/auth/login`, and `/api/health` require `Authorization: Bearer <token>`.

```
POST   /api/auth/register
POST   /api/auth/login
GET    /api/auth/me
POST   /api/auth/link-token
POST   /api/auth/revoke-tokens

GET    /api/subscriptions
POST   /api/subscriptions
PUT    /api/subscriptions/:id
DELETE /api/subscriptions/:id
PATCH  /api/subscriptions/:id/toggle

GET    /api/forecast?months=3

POST   /api/monobank/token
DELETE /api/monobank/token
POST   /api/monobank/sync
GET    /api/monobank/status

POST   /api/import

GET    /api/health
```

## Telegram bot

Commands: `/subscriptions`, `/upcoming`

To link: Settings → generate token → send `/start <token>` to the bot
Reminders run at 09:00 daily by default (configurable via `NOTIFICATION_HOUR` / `NOTIFICATION_MINUTE`), 3 days before billing date by default (configurable per-user in Settings)