const axios = require('axios');
const Transaction = require('../models/Transaction');

const MONO_BASE = 'https://api.monobank.ua';

// check token (get client info)
exports.verifyToken = async (token) => {
  const res = await axios.get(`${MONO_BASE}/personal/client-info`, {
    headers: { 'X-Token': token },
  });
  return res.data;
};

// fetch transactions for an account
// ! mono limits: no more than 1 request per 60 seconds, and max 31 days per request
exports.fetchTransactions = async (token, from, to = new Date()) => {
  // fetch list of accounts
  const clientInfo = await exports.verifyToken(token);
  const accounts = clientInfo.accounts;

  const uahAccount = accounts.find(a => a.currencyCode === 980) || accounts[0];

  // fetch transactions in chunks if needed
  const chunks = splitDateRange(from, to, 31);
  const allTransactions = [];

  for (const [chunkFrom, chunkTo] of chunks) {
    const fromTs = Math.floor(chunkFrom.getTime() / 1000);
    const toTs = Math.floor(chunkTo.getTime() / 1000);

    const { data } = await axios.get(
      `${MONO_BASE}/personal/statement/${uahAccount.id}/${fromTs}/${toTs}`,
      { headers: { 'X-Token': token } }
    );
    allTransactions.push(...data);

    // deeeelay to avoid exceeeeeding the limit
    if (chunks.length > 1) await delay(61_000);
  }

  return allTransactions;
};

// save transactions to DB, ignoring dups
exports.saveTransactions = async (userId, transactions) => {
  const ops = transactions.map(t => ({
    updateOne: {
      filter: { externalId: t.id },
      update: {
        $setOnInsert: {
          userId,
          externalId: t.id,
          source: 'monobank',
          amount: t.amount,
          currency: 'UAH',
          description: t.description,
          mcc: t.mcc,
          date: new Date(t.time * 1000),
        },
      },
      upsert: true,
    },
  }));

  await Transaction.bulkWrite(ops);
};

// helper func
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