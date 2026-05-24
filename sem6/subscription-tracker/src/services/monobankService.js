const axios = require('axios');
const Transaction = require('../models/Transaction');

const BASE_URL = 'https://api.monobank.ua';

exports.verifyToken = async (token) => {
  const res = await axios.get(`${BASE_URL}/personal/client-info`, {
    headers: { 'X-Token': token },
  });
  return res.data;
};

exports.fetchTransactions = async (token, from, to = new Date()) => {
  const accounts = await exports.verifyToken(token);
  const accountId = accounts.accounts.find(a => a.currencyCode === 980)?.id
    || accounts.accounts[0]?.id;

  if (!accountId) throw new Error('No account found');

  const chunks = splitDateRange(from, to, 31);
  const all = [];

  for (let i = 0; i < chunks.length; i++) {
    const [start, end] = chunks[i];
    const fromTs = Math.floor(start.getTime() / 1000);
    const toTs   = Math.floor(end.getTime()   / 1000);

    const res = await axios.get(
      `${BASE_URL}/personal/statement/${accountId}/${fromTs}/${toTs}`,
      { headers: { 'X-Token': token } }
    );
    all.push(...res.data);

      if (i < chunks.length - 1) await delay(61_000); // monobank rate limit: 1 req/60s
  }

  return all;
};

exports.saveTransactions = async (userId, transactions) => {
  if (!transactions.length) return;

  const ops = transactions.map(t => ({
    updateOne: {
      filter: { externalId: t.id },
      update: {
        $setOnInsert: {
          userId,
          externalId:  t.id,
          source:      'monobank',
          amount:      t.amount,
          currency:    'UAH',
          description: t.description || '',
          mcc:         t.mcc,
          date:        new Date(t.time * 1000),
        },
        $set: { counterName: t.counterName || '' },
      },
      upsert: true,
    },
  }));

  await Transaction.bulkWrite(ops);
};

function splitDateRange(from, to, maxDays) {
  const chunks = [];
  let current = new Date(from);
  while (current < to) {
    const chunkEnd = new Date(Math.min(
      current.getTime() + maxDays * 24 * 60 * 60 * 1000,
      to.getTime()
    ));
    chunks.push([new Date(current), chunkEnd]);
    current = new Date(chunkEnd.getTime() + 1);
  }
  return chunks;
}

const delay = (ms) => new Promise(resolve => setTimeout(resolve, ms));