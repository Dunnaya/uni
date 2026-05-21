const csv = require('csv-parse/sync');
const xlsx = require('xlsx');

const BANK_ADAPTERS = {
  privatbank:   parsePrivatbank,
  monobank_csv: parseMonobankCsv,
  generic:      parseGeneric,
};

exports.parseFile = (buffer, filename, bank = 'generic') => {
  const ext = filename.split('.').pop().toLowerCase();

  let rows;
  if (ext === 'csv') {
    rows = csv.parse(buffer, { columns: true, skip_empty_lines: true, bom: true });
  } else if (['xls', 'xlsx'].includes(ext)) {
    // cellDates: true converts Excel date serials to JS Date objects
    const wb = xlsx.read(buffer, { cellDates: true });
    rows = xlsx.utils.sheet_to_json(wb.Sheets[wb.SheetNames[0]]);
  } else {
    throw new Error('Only CSV and XLSX files are supported');
  }

  const adapter = BANK_ADAPTERS[bank] || BANK_ADAPTERS.generic;
  return rows.map(adapter).filter(Boolean);
};

function parseDate(val) {
  if (!val) return null;
  if (val instanceof Date) return val;
  // Excel sometimes returns numbers (serial dates) — xlsx handles those with cellDates:true
  // but just in case
  if (typeof val === 'number') return new Date((val - 25569) * 86400 * 1000);
  return new Date(val);
}

function parsePrivatbank(row) {
  const amount = parseFloat(String(row['Сума'] || row['Дебет'] || 0).replace(',', '.'));
  if (isNaN(amount) || amount >= 0) return null;

  return {
    date: parseDate(row['Дата операції'] || row['Дата']),
    description: row['Опис'] || row['Призначення'] || '',
    amount: Math.round(amount * 100),
    currency: row['Валюта'] || 'UAH',
  };
}

function parseMonobankCsv(row) {
  const amount = parseFloat(String(row['Сума'] || 0).replace(',', '.'));
  if (isNaN(amount) || amount >= 0) return null;

  return {
    date: parseDate(row['Дата']),
    description: row['Опис'] || '',
    amount: Math.round(amount * 100),
    currency: row['Валюта операції'] || 'UAH',
    mcc: parseInt(row['MCC']) || null,
  };
}

function parseGeneric(row) {
  const keys = Object.keys(row);
  const dateKey   = keys.find(k => /дата|date/i.test(k));
  const descKey   = keys.find(k => /опис|description|призначення/i.test(k));
  const amountKey = keys.find(k => /сума|amount|деб/i.test(k));

  if (!dateKey || !amountKey) return null;

  const amount = parseFloat(String(row[amountKey] || 0).replace(',', '.'));
  if (isNaN(amount) || amount >= 0) return null;

  return {
    date: parseDate(row[dateKey]),
    description: descKey ? row[descKey] : '',
    amount: Math.round(amount * 100),
    currency: 'UAH',
  };
}