const csv = require('csv-parse/sync');
const xlsx = require('xlsx');

exports.parseFile = (buffer, filename) => {
  const ext = filename.split('.').pop().toLowerCase();

  if (ext === 'csv') {
    const rows = csv.parse(buffer, { columns: true, skip_empty_lines: true, bom: true });
    return parseByKeys(rows);
  }

  if (['xls', 'xlsx'].includes(ext)) {
    const wb = xlsx.read(buffer, { cellDates: true });
    const ws = wb.Sheets[wb.SheetNames[0]];
    const raw = xlsx.utils.sheet_to_json(ws, { header: 1 });

    // Find the header row — first row with 5+ non-empty cells
    const headerIdx = raw.findIndex(r => (r || []).filter(v => v != null && v !== '').length >= 5);
    if (headerIdx === -1) throw new Error('Could not find header row in the file');

    const rows = xlsx.utils.sheet_to_json(ws, { cellDates: true, range: headerIdx });
    return parseByKeys(rows);
  }

  throw new Error('Only CSV and XLSX/XLS files are supported');
};

// Handles all Monobank formats (UA/EN, CSV/XLS) and PrivatBank XLSX
function normalizeKeys(rows) {
  return rows.map(row => {
    const normalized = {};
    for (const [k, v] of Object.entries(row)) normalized[k.normalize('NFC')] = v;
    return normalized;
  });
}

function parseByKeys(rows) {
  rows = normalizeKeys(rows);
  if (!rows.length) return [];
  const keys = Object.keys(rows[0]);

  if (keys.includes('Сума в валюті картки (UAH)') || keys.includes('Card currency amount, (UAH)')) {
    return rows.map(parseMonobank).filter(Boolean);
  }
  if (keys.includes('Сума в валюті картки') && keys.includes('Валюта картки')) {
    return rows.map(parsePrivatbank).filter(Boolean);
  }
  return rows.map(parseGeneric).filter(Boolean);
}

function parseMonobank(row) {
  const amountRaw = row['Сума в валюті картки (UAH)'] ?? row['Card currency amount, (UAH)'];
  const amount = typeof amountRaw === 'number'
    ? amountRaw
    : parseFloat(String(amountRaw || 0).replace(',', '.'));

  if (isNaN(amount) || amount >= 0) return null;

  return {
    date:        parseDate(row['Дата i час операції'] || row['Date and time']),
    description: row['Деталі операції'] || row['Description'] || '',
    amount:      Math.round(amount * 100),
    currency:    'UAH',
    mcc:         parseInt(row['MCC']) || null,
  };
}

function parsePrivatbank(row) {
  const amountRaw = row['Сума в валюті картки'];
  const amount = typeof amountRaw === 'number'
    ? amountRaw
    : parseFloat(String(amountRaw || 0).replace(',', '.'));

  if (isNaN(amount) || amount >= 0) return null;

  return {
    date:        parseDate(row['Дата'] || row['Дата операції']),
    description: row['Опис операції'] || row['Опис'] || '',
    amount:      Math.round(amount * 100),
    currency:    row['Валюта картки'] || 'UAH',
  };
}

function parseGeneric(row) {
  const keys = Object.keys(row);
  const dateKey   = keys.find(k => /дата|date/i.test(k));
  const descKey   = keys.find(k => /деталі|опис|description|призначення/i.test(k));
  const amountKey = keys.find(k => /сума.*картк|card.*amount|сума$|amount$/i.test(k))
                 || keys.find(k => /сума|amount|деб/i.test(k));

  if (!dateKey || !amountKey) return null;

  const raw = row[amountKey];
  const amount = typeof raw === 'number'
    ? raw
    : parseFloat(String(raw || 0).replace(',', '.'));

  if (isNaN(amount) || amount >= 0) return null;

  return {
    date:        parseDate(row[dateKey]),
    description: descKey ? String(row[descKey]) : '',
    amount:      Math.round(amount * 100),
    currency:    'UAH',
  };
}

function parseDate(val) {
  if (!val) return null;
  if (val instanceof Date) return val;
  if (typeof val === 'number') return new Date((val - 25569) * 86400 * 1000);
  const str = String(val).trim();
  const dmy = str.match(/^(\d{2})\.(\d{2})\.(\d{4})(?: (.+))?$/);
  if (dmy) {
    const [, dd, mm, yyyy, time = '00:00:00'] = dmy;
    return new Date(`${yyyy}-${mm}-${dd}T${time}`);
  }
  return new Date(str);
}