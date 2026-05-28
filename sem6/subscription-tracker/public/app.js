// state
let token = localStorage.getItem('st_token');
let subs = [];
let editId = null;
let fcMonths = 3;
let authMode = 'login';

const MONTHS = ['Jan','Feb','Mar','Apr','May','Jun','Jul','Aug','Sep','Oct','Nov','Dec'];
const CYCLES = { monthly: '/ mo', yearly: '/ yr', weekly: '/ wk', custom: '' };

// approximate exchange rates for UAH display (same as backend)
const RATES = { UAH: 1, USD: 41, EUR: 44 };
const toUAH = (amount, currency) => amount * (RATES[currency] || 1);
const CAT_COLOR = {
  entertainment: '#7c3aed', work: '#4169e1', cloud: '#0891b2',
  music: '#db2777', gaming: '#f59e0b', other: '#94a3b8'
};

// --- api helper ---

async function api(method, path, body, isForm) {
  const headers = {};
  if (token) headers['Authorization'] = 'Bearer ' + token;
  if (!isForm) headers['Content-Type'] = 'application/json';

  const res = await fetch('/api' + path, {
    method,
    headers,
    body: isForm ? body : (body ? JSON.stringify(body) : undefined),
  });

  const data = await res.json().catch(() => ({}));
  if (res.status === 401) {
    // Token expired or revoked — clear session and return to login
    signOut();
    throw new Error(data.error || 'Session expired. Please log in again.');
  }
  if (!res.ok) throw new Error(data.error || 'Request failed');
  return data;
}

// --- toast ---

function toast(msg, isErr) {
  const el = document.getElementById('toast');
  el.textContent = msg;
  el.className = 'toast show' + (isErr ? ' err' : '');
  clearTimeout(el._t);
  el._t = setTimeout(() => {
    el.classList.add('hide');
    setTimeout(() => { el.className = 'toast' + (isErr ? ' err' : ''); }, 300);
  }, 4500);
}

// --- auth ---

function setMode(m) {
  authMode = m;
  document.querySelectorAll('.tab').forEach((t, i) => {
    t.classList.toggle('active', i === (m === 'login' ? 0 : 1));
  });
  document.getElementById('a-btn').textContent = m === 'login' ? 'Sign in' : 'Create account';
  document.getElementById('a-err').style.display = 'none';
}

async function handleAuth() {
  const email = document.getElementById('a-email').value.trim();
  const password = document.getElementById('a-pass').value;
  const err = document.getElementById('a-err');
  err.style.display = 'none';

  try {
    const data = await api('POST', authMode === 'login' ? '/auth/login' : '/auth/register', { email, password });
    token = data.token;
    localStorage.setItem('st_token', token);
    initApp();
  } catch (e) {
    err.textContent = e.message;
    err.style.display = 'block';
  }
}

document.addEventListener('keydown', e => {
  if (e.key === 'Enter' && !document.getElementById('app').classList.contains('show')) {
    handleAuth();
  }
});

function signOut() {
  token = null;
  localStorage.removeItem('st_token');

  // Reset all stateful UI elements so the next login starts clean
  const csvMsg = document.getElementById('csv-msg');
  if (csvMsg) { csvMsg.style.display = 'none'; csvMsg.textContent = ''; }

  const monoMsg = document.getElementById('mono-msg');
  if (monoMsg) { monoMsg.style.display = 'none'; monoMsg.textContent = ''; }

  const monoStatus = document.getElementById('mono-status');
  if (monoStatus) monoStatus.style.display = 'none';

  // Reset any toast / notification state
  subs = [];

  document.getElementById('app').classList.remove('show');
  document.getElementById('auth').style.display = 'flex';
}

// --- init ---

async function initApp() {
  document.getElementById('auth').style.display = 'none';
  document.getElementById('app').classList.add('show');

  try {
    const me = await api('GET', '/auth/me');
    document.getElementById('s-email').textContent = me.email;
    document.getElementById('tg-linked').style.display = me.telegramLinked ? 'block' : 'none';
  } catch {}

  go('dashboard');
  loadMonoStatus();
}

// --- navigation ---

function go(page) {
  document.querySelectorAll('.page').forEach(p => p.classList.remove('active'));
  document.querySelectorAll('.nav-btn[data-page]').forEach(b => b.classList.remove('active'));
  document.getElementById('page-' + page).classList.add('active');
  document.querySelector('[data-page="' + page + '"]').classList.add('active');

  if (page === 'dashboard')     loadDashboard();
  if (page === 'subscriptions') loadSubs();
  if (page === 'forecast')      loadForecast();
}

// --- dashboard ---

async function loadDashboard() {
  try {
    subs = await api('GET', '/subscriptions');
    const active = subs.filter(s => s.isActive);

    // monthly total converted to UAH (approx. rates)
    const monthly = active.reduce((sum, s) => {
      const m = s.billingCycle === 'yearly' ? s.amount / 12
              : s.billingCycle === 'weekly' ? s.amount * 4.33
              : s.amount;
      return sum + toUAH(m, s.currency);
    }, 0);

    const next = [...active].sort((a, b) => new Date(a.nextBillingDate) - new Date(b.nextBillingDate))[0];

    document.getElementById('stats-row').innerHTML = `
      <div class="stat">
        <div class="stat-label">Monthly spend (UAH)</div>
        <div class="stat-value">₴${monthly.toFixed(0)}</div>
        <div class="stat-hint">${active.length} active subscription${active.length !== 1 ? "s" : ""}</div>
      </div>
      <div class="stat">
        <div class="stat-label">Active subscriptions</div>
        <div class="stat-value">${active.length}</div>
        <div class="stat-hint">out of ${subs.length} total</div>
      </div>
      <div class="stat">
        <div class="stat-label">Next billing</div>
        <div class="stat-value" style="font-size:17px">${next ? fmtDate(next.nextBillingDate) : '—'}</div>
        <div class="stat-hint">${next ? next.name + ' · ' + fmtMoney(next.amount, next.currency) : 'no upcoming'}</div>
      </div>
      <div class="stat">
        <div class="stat-label">Annual estimate (UAH)</div>
        <div class="stat-value">₴${(monthly * 12).toFixed(0)}</div>
        <div class="stat-hint">at current rates</div>
      </div>
    `;

    // upcoming 7 days
    const now = new Date();
    const in7 = new Date(now.getTime() + 7 * 864e5);
    const coming = active
      .filter(s => new Date(s.nextBillingDate) >= now && new Date(s.nextBillingDate) <= in7)
      .sort((a, b) => new Date(a.nextBillingDate) - new Date(b.nextBillingDate));

    if (!coming.length) {
      document.getElementById('upcoming').innerHTML = '<p style="color:#94a3b8;font-size:13px">No charges in the next 7 days ✓</p>';
    } else {
      document.getElementById('upcoming').innerHTML = coming.map(s => {
        const billing = new Date(s.nextBillingDate);
        billing.setHours(0, 0, 0, 0);
        const today = new Date(); today.setHours(0, 0, 0, 0);
        const days = Math.ceil((billing - today) / 864e5);
        const badgeClass = days <= 1 ? 'badge-red' : days <= 3 ? 'badge-warn' : 'badge-ok';
        const dayLabel = days === 0 ? 'today' : days === 1 ? 'tomorrow' : days + 'd';
        return `<div class="upcoming-item">
          <div class="dot dot-${s.category}"></div>
          <div>
            <div style="font-weight:500">${s.name}</div>
            <div style="font-size:12px;color:#94a3b8">${fmtDate(s.nextBillingDate)}</div>
          </div>
          <div style="margin-left:auto;font-weight:600">
            ${fmtMoney(s.amount, s.currency)}
            <span class="badge ${badgeClass}">${dayLabel}</span>
          </div>
        </div>`;
      }).join('');
    }

    // category breakdown in UAH equivalent (all currencies converted)
    const totals = {};
    active.forEach(s => {
      totals[s.category] = (totals[s.category] || 0) + toUAH(s.amount, s.currency);
    });
    const total = Object.values(totals).reduce((a, b) => a + b, 1);

    document.getElementById('by-category').innerHTML = Object.entries(totals)
      .sort((a, b) => b[1] - a[1])
      .map(([cat, amt]) => `
        <div class="cat-bar">
          <div class="cat-bar-top">
            <span style="font-weight:500">${cat}</span>
            <span>₴${amt.toFixed(0)}</span>
          </div>
          <div class="bar-track">
            <div class="bar-fill" style="width:${(amt / total * 100).toFixed(1)}%;background:${CAT_COLOR[cat]}"></div>
          </div>
        </div>
      `).join('') || '<p style="color:#94a3b8;font-size:13px">No data</p>';

  } catch (e) {
    toast(e.message, true);
  }
}

// --- subscriptions ---

async function loadSubs() {
  try {
    subs = await api('GET', '/subscriptions');
    renderTable();
  } catch (e) {
    toast(e.message, true);
  }
}

function renderTable() {
  const q      = document.getElementById('q').value.toLowerCase();
  const cat    = document.getElementById('f-cat').value;
  const status = document.getElementById('f-status').value;

  const filtered = subs.filter(s => {
    if (q && !s.name.toLowerCase().includes(q)) return false;
    if (cat && s.category !== cat) return false;
    if (status === 'active' && !s.isActive) return false;
    if (status === 'paused' && s.isActive) return false;
    return true;
  });

  if (!filtered.length) {
    document.getElementById('subs-table').innerHTML = '<div class="empty">No subscriptions found</div>';
    return;
  }

  const rows = filtered.map(s => `
    <tr class="${s.isActive ? '' : 'paused-row'}">
      <td style="font-weight:500">
        ${s.name}
        ${s.isTrial ? '<span class="badge badge-warn" style="margin-left:4px">trial</span>' : ''}
      </td>
      <td><span class="cat-badge ${s.category}">${s.category}</span></td>
      <td>${fmtMoney(s.amount, s.currency)} <span style="color:#94a3b8">${CYCLES[s.billingCycle] || ''}</span></td>
      <td>${fmtDate(s.nextBillingDate)}</td>
      <td>
        <span class="badge ${s.isActive ? 'badge-ok' : ''}" style="${!s.isActive ? 'background:#f8fafc;color:#94a3b8' : ''}">
          ${s.isActive ? 'active' : 'paused'}
        </span>
      </td>
      <td style="white-space:nowrap">
        <button class="btn btn-default btn-sm" onclick="openModal('${s._id}')">Edit</button>
        <button class="btn btn-default btn-sm" onclick="toggleSub('${s._id}')">${s.isActive ? 'Pause' : 'Resume'}</button>
        <button class="btn btn-danger btn-sm" onclick="deleteSub('${s._id}', '${s.name.replace(/'/g, "\\'")}')">Delete</button>
      </td>
    </tr>
  `).join('');

  document.getElementById('subs-table').innerHTML = `
    <table>
      <thead>
        <tr>
          <th>Name</th><th>Category</th><th>Amount</th><th>Next billing</th><th>Status</th><th></th>
        </tr>
      </thead>
      <tbody>${rows}</tbody>
    </table>
  `;
}

// --- add/edit modal ---

function openModal(id) {
  editId = id || null;
  document.getElementById('modal-title').textContent = id ? 'Edit subscription' : 'New subscription';

  const s = id ? subs.find(x => x._id === id) : null;

  document.getElementById('m-name').value    = s?.name || '';
  document.getElementById('m-amount').value  = s?.amount || '';
  document.getElementById('m-cur').value     = s?.currency || 'UAH';
  document.getElementById('m-cycle').value   = s?.billingCycle || 'monthly';
  document.getElementById('m-cat').value     = s?.category || 'entertainment';
  document.getElementById('m-desc').value    = s?.description || '';
  document.getElementById('m-trial').checked = s?.isTrial || false;
  document.getElementById('m-days').value     = s?.customCycleDays || '';
  document.getElementById('m-reminder').value = s?.reminderDays || '';

  if (s) {
    document.getElementById('m-date').value = new Date(s.nextBillingDate).toISOString().split('T')[0];
  } else {
    const d = new Date();
    d.setMonth(d.getMonth() + 1);
    document.getElementById('m-date').value = d.toISOString().split('T')[0];
  }

  toggleCustomDays();
  ['name', 'amount', 'date'].forEach(f => document.getElementById('e-' + f).style.display = 'none');
  if (!id) document.getElementById('m-reminder').value = '';
  document.getElementById('modal').classList.add('open');
}

function closeModal() {
  document.getElementById('modal').classList.remove('open');
}

function toggleCustomDays() {
  const show = document.getElementById('m-cycle').value === 'custom';
  document.getElementById('m-custom-wrap').style.display = show ? 'block' : 'none';
}

async function saveSub() {
  let ok = true;
  const name   = document.getElementById('m-name').value.trim();
  const amount = parseFloat(document.getElementById('m-amount').value);
  const date   = document.getElementById('m-date').value;

  if (!name)           { document.getElementById('e-name').style.display   = 'block'; ok = false; }
  if (!amount || amount <= 0) { document.getElementById('e-amount').style.display = 'block'; ok = false; }
  if (!date)           { document.getElementById('e-date').style.display   = 'block'; ok = false; }
  if (!ok) return;

  const body = {
    name, amount,
    currency:       document.getElementById('m-cur').value,
    billingCycle:   document.getElementById('m-cycle').value,
    category:       document.getElementById('m-cat').value,
    description:    document.getElementById('m-desc').value,
    isTrial:        document.getElementById('m-trial').checked,
    nextBillingDate: date,
    customCycleDays: parseInt(document.getElementById('m-days').value) || null,
    reminderDays:    parseInt(document.getElementById('m-reminder').value) || null,
  };

  try {
    if (editId) await api('PUT', '/subscriptions/' + editId, body);
    else        await api('POST', '/subscriptions', body);
    closeModal();
    toast(editId ? 'Updated' : 'Subscription added');
    loadSubs();
  } catch (e) {
    toast(e.message, true);
  }
}

async function toggleSub(id) {
  try {
    await api('PATCH', '/subscriptions/' + id + '/toggle');
    loadSubs();
  } catch (e) {
    toast(e.message, true);
  }
}

async function deleteSub(id, name) {
  if (!confirm('Delete "' + name + '"?')) return;
  try {
    await api('DELETE', '/subscriptions/' + id);
    toast('Deleted');
    subs = subs.filter(s => s._id !== id);
    renderTable();
  } catch (e) {
    toast(e.message, true);
  }
}

// --- forecast ---

async function loadForecast() {
  try {
    const data = await api('GET', '/forecast?months=' + fcMonths);
    const summary = data.monthlySummary || {};
    const events  = data.events || [];

    document.getElementById('fc-grid').innerHTML = Object.entries(summary).map(([key, currencies]) => {
      const [y, m] = key.split('-');
      const count = events.filter(e => e.date.startsWith(key)).length;
      // monthlySummary is { UAH: 59, USD: 9.99 } — show each currency on its own line
      const SYMBOLS = { UAH: '₴', USD: '$', EUR: '€', GBP: '£' };
      const currencyLines = Object.entries(currencies).map(([c, v]) => {
        const sym = SYMBOLS[c] || c;
        return `<div class="fc-currency-row"><span class="fc-sym">${sym}</span><span class="fc-val">${v.toFixed(0)}</span><span class="fc-cur">${c}</span></div>`;
      }).join('');
      return `<div class="fc-card">
        <div class="fc-month">${MONTHS[+m - 1]} ${y}</div>
        <div class="fc-amounts">${currencyLines}</div>
        <div class="fc-count">${count} charge${count !== 1 ? 's' : ''}</div>
      </div>`;
    }).join('') || '<p style="color:#94a3b8">No data</p>';

    document.getElementById('fc-events').innerHTML = events.map(e => `
      <div class="event-row">
        <div class="dot dot-${e.category}"></div>
        <div style="flex:1;font-weight:500">${e.name}</div>
        <div style="color:#94a3b8;font-size:12px">${fmtDate(e.date)}</div>
        <div style="font-weight:600;margin-left:10px">${fmtMoney(e.amount, e.currency)}</div>
      </div>
    `).join('') || '<p style="color:#94a3b8;font-size:13px">No events</p>';

  } catch (e) {
    toast(e.message, true);
  }
}

function setMonths(n, btn) {
  fcMonths = n;
  document.querySelectorAll('.m-tab').forEach(b => b.classList.remove('active'));
  btn.classList.add('active');
  loadForecast();
}

// --- import ---

function onDrag(e) { e.preventDefault(); document.getElementById('drop-zone').classList.add('over'); }
function offDrag()  { document.getElementById('drop-zone').classList.remove('over'); }

function onDrop(e) {
  e.preventDefault();
  offDrag();
  if (e.dataTransfer.files[0]) uploadCsv({ files: e.dataTransfer.files });
}

function uploadCsv(input) {
  const file = input.files[0];
  if (!file) return;

  const fd = new FormData();
  fd.append('file', file);
  fd.append('bank', 'monobank_csv');

  const msg = document.getElementById('csv-msg');

  api('POST', '/import', fd, true)
    .then(d => {
      const newCount     = d.newSubscriptions     || 0;
      const updatedCount = d.updatedSubscriptions || 0;
      let text = `Parsed ${d.parsed} rows`;
      if (newCount > 0)     text += ` — ${newCount} new subscription${newCount !== 1 ? 's' : ''} detected`;
      if (updatedCount > 0) text += ` — ${updatedCount} subscription${updatedCount !== 1 ? 's' : ''} updated`;
      if (newCount === 0 && updatedCount === 0) text += ' — no subscriptions detected';
      msg.className = 'result-msg result-ok';
      msg.textContent = text;
      msg.style.display = 'block';
      toast('Import done');
    })
    .catch(e => {
      msg.className = 'result-msg result-err';
      msg.textContent = e.message;
      msg.style.display = 'block';
    });
}

async function loadMonoStatus() {
  try {
    const s = await api('GET', '/monobank/status');
    if (s.connected) {
      document.getElementById('mono-status').style.display = 'block';
      document.getElementById('sync-btn').style.display = 'inline-flex';
      document.getElementById('mono-token').style.display = 'none';
      document.getElementById('mono-save-btn').style.display = 'none';
      if (s.lastSync) {
        document.getElementById('mono-sync-date').textContent =
          'Last sync: ' + new Date(s.lastSync).toLocaleDateString();
      }
    } else {
      document.getElementById('mono-status').style.display = 'none';
      document.getElementById('sync-btn').style.display = 'none';
      document.getElementById('mono-token').style.display = 'block';
      document.getElementById('mono-save-btn').style.display = 'inline-flex';
      document.getElementById('mono-sync-date').textContent = '';
    }
  } catch {}
}

async function saveMonoToken() {
  const t   = document.getElementById('mono-token').value.trim();
  const msg = document.getElementById('mono-msg');
  if (!t) return;

  try {
    await api('POST', '/monobank/token', { token: t });
    msg.className = 'result-msg result-ok';
    msg.textContent = 'Token saved and verified';
    msg.style.display = 'block';
    document.getElementById('sync-btn').style.display = 'inline-flex';
    document.getElementById('mono-status').style.display = 'block';
    document.getElementById('mono-token').style.display = 'none';
    document.getElementById('mono-save-btn').style.display = 'none';
    toast('Monobank connected');
  } catch (e) {
    msg.className = 'result-msg result-err';
    msg.textContent = e.message + ' — check your token';
    msg.style.display = 'block';
  }
}

async function syncMono() {
  const btn = document.getElementById('sync-btn');
  btn.textContent = 'Syncing...';
  btn.disabled = true;

  try {
    const d = await api('POST', '/monobank/sync');
    toast(`Synced ${d.synced} transactions · ${d.newSubscriptions} new subscriptions`);
  } catch (e) {
    toast(e.message, true);
  } finally {
    btn.textContent = 'Sync now';
    btn.disabled = false;
  }
}

// --- settings ---

async function getTgToken() {
  try {
    const d = await api('POST', '/auth/link-token');
    document.getElementById('tg-token-box').textContent = d.token;
    document.getElementById('tg-token-box').style.display = 'block';
    document.getElementById('tg-cmd').textContent = '/start ' + d.token;
    document.getElementById('tg-instr').style.display = 'block';
    toast('Token generated — valid for 15 min');
  } catch (e) {
    toast(e.message, true);
  }
}

// --- helpers ---

function fmtDate(d) {
  if (!d) return '—';
  const dt = new Date(d);
  return dt.getDate() + ' ' + MONTHS[dt.getMonth()] + ' ' + dt.getFullYear();
}

function fmtMoney(n, cur) {
  const sym = cur === 'USD' ? '$' : cur === 'EUR' ? '€' : '₴';
  return sym + Number(n).toFixed(2).replace(/\.00$/, '');
}

// --- start ---

if (token) {
  initApp();
} else {
  document.getElementById('auth').style.display = 'flex';
}