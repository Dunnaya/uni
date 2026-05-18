import React, { useState, useEffect } from 'react';
import './App.css';

const API = 'http://localhost:8080/api';

function App() {
  const [products, setProducts]       = useState([]);
  const [cart, setCart]               = useState([]);
  const [orders, setOrders]           = useState([]);
  const [message, setMessage]         = useState('');
  const [token, setToken]             = useState('');
  const [role, setRole]               = useState('');
  const [userId, setUserId]           = useState(null);
  const [username, setUsername]       = useState('');
  const [password, setPassword]       = useState('');
  const [loggedIn, setLoggedIn]       = useState(false);

  const [newProduct, setNewProduct]   = useState({ name: '', price: '', stockQuantity: '' });
  const [editId, setEditId]           = useState(null);
  const [editProduct, setEditProduct] = useState({});

  const authHeaders = { 'Content-Type': 'application/json', Authorization: token };

  const loadProducts = () =>
    fetch(`${API}/products`).then(r => r.json()).then(setProducts);

  const loadOrders = (bearer) =>
    fetch(`${API}/orders`, { headers: { Authorization: bearer || token } })
      .then(r => r.json()).then(setOrders);

  useEffect(() => { loadProducts(); }, []);

  const login = () => {
    fetch(`${API}/auth/login`, {
      method: 'POST',
      headers: { 'Content-Type': 'application/json' },
      body: JSON.stringify({ username, password })
    })
      .then(r => r.json())
      .then(data => {
        if (data.token) {
          const bearer = 'Bearer ' + data.token;
          setToken(bearer);
          setRole(data.role);
          setUserId(data.userId);
          setLoggedIn(true);
          setMessage('Logged in as ' + username + ' (' + data.role + ')');
          if (data.role === 'ADMIN') loadOrders(bearer);
        } else {
          setMessage('Wrong username or password');
        }
      })
      .catch(() => setMessage('Connection error'));
  };

  const logout = () => {
    setToken(''); setRole(''); setUserId(null);
    setLoggedIn(false); setCart([]); setOrders([]);
    setMessage('');
  };

  const addToCart = (product) => {
    setCart(prev => {
      const ex = prev.find(i => i.productId === product.id);
      if (ex) return prev.map(i => i.productId === product.id ? { ...i, quantity: i.quantity + 1 } : i);
      return [...prev, { productId: product.id, quantity: 1, name: product.name, price: product.price }];
    });
  };

  const placeOrder = () => {
    if (!loggedIn) { setMessage('Please log in first'); return; }
    if (cart.length === 0) return;
    fetch(`${API}/orders`, {
      method: 'POST',
      headers: authHeaders,
      body: JSON.stringify({ userId, items: cart.map(i => ({ productId: i.productId, quantity: i.quantity })) })
    })
      .then(r => r.json())
      .then(data => {
        if (data.id) {
          setMessage('Order #' + data.id + ' created. Total: $' + data.totalPrice + '. Click Pay below.');
          setOrders(prev => [...prev, data]);
          setCart([]);
          loadProducts();
        } else {
          setMessage('Error: ' + data.error);
        }
      })
      .catch(() => setMessage('Connection error'));
  };

  const payOrder = (orderId) => {
    fetch(`${API}/orders/${orderId}`, { method: 'PUT', headers: authHeaders })
      .then(r => r.json())
      .then(() => {
        setMessage('Order #' + orderId + ' paid');
        setOrders(prev => prev.filter(o => o.id !== orderId));
      })
      .catch(() => setMessage('Payment error'));
  };

  const addProduct = () => {
    fetch(`${API}/products`, {
      method: 'POST', headers: authHeaders,
      body: JSON.stringify({ name: newProduct.name, price: parseFloat(newProduct.price), stockQuantity: parseInt(newProduct.stockQuantity) })
    })
      .then(r => r.json())
      .then(() => { loadProducts(); setNewProduct({ name: '', price: '', stockQuantity: '' }); setMessage('Product added'); });
  };

  const saveEdit = (id) => {
    fetch(`${API}/products/${id}`, {
      method: 'PUT', headers: authHeaders,
      body: JSON.stringify({ name: editProduct.name, price: parseFloat(editProduct.price), stockQuantity: parseInt(editProduct.stockQuantity) })
    })
      .then(r => r.json())
      .then(() => { loadProducts(); setEditId(null); setMessage('Product updated'); });
  };

  const deleteProduct = (id) => {
    fetch(`${API}/products/${id}`, { method: 'DELETE', headers: authHeaders })
      .then(() => { loadProducts(); setMessage('Product deleted'); });
  };

  const blacklist = (uid, action) => {
    fetch(`${API}/admin/blacklist/${uid}?action=${action}`, { method: 'PUT', headers: authHeaders })
      .then(r => r.json())
      .then(() => {
        setMessage('User ' + (action === 'block' ? 'blocked' : 'unblocked'));
        loadOrders();
      });
  };

  return (
    <div className="app">
      <h1 className="app-title">Shop</h1>

      {!loggedIn ? (
        <div className="login-box">
          <h2>Login</h2>
          <div className="login-form">
            <input className="input" placeholder="Username" value={username} onChange={e => setUsername(e.target.value)} />
            <input className="input" placeholder="Password" type="password" value={password} onChange={e => setPassword(e.target.value)} />
            <button className="btn btn-primary" onClick={login}>Login</button>
          </div>
          <p className="hint">Test accounts: admin / password &nbsp;|&nbsp; client1 / password</p>
        </div>
      ) : (
        <div className="user-bar">
          <span className="user-info">{username} ({role})</span>
          <button className="btn btn-secondary" onClick={logout}>Logout</button>
        </div>
      )}

      {role === 'ADMIN' && (
        <>
          <h2>Product Management</h2>
          <div className="add-product-form">
            <input className="input" placeholder="Name" value={newProduct.name} onChange={e => setNewProduct({ ...newProduct, name: e.target.value })} />
            <input className="input input-sm" placeholder="Price" type="number" value={newProduct.price} onChange={e => setNewProduct({ ...newProduct, price: e.target.value })} />
            <input className="input input-sm" placeholder="Qty" type="number" value={newProduct.stockQuantity} onChange={e => setNewProduct({ ...newProduct, stockQuantity: e.target.value })} />
            <button className="btn btn-success" onClick={addProduct}>Add Product</button>
          </div>

          <table className="table">
            <thead>
              <tr><th>Name</th><th>Price</th><th>Qty</th><th>Actions</th></tr>
            </thead>
            <tbody>
              {products.map(p => (
                <tr key={p.id}>
                  {editId === p.id ? (
                    <>
                      <td><input className="input" value={editProduct.name} onChange={e => setEditProduct({ ...editProduct, name: e.target.value })} /></td>
                      <td><input className="input input-sm" type="number" value={editProduct.price} onChange={e => setEditProduct({ ...editProduct, price: e.target.value })} /></td>
                      <td><input className="input input-sm" type="number" value={editProduct.stockQuantity} onChange={e => setEditProduct({ ...editProduct, stockQuantity: e.target.value })} /></td>
                      <td className="actions">
                        <button className="btn btn-success" onClick={() => saveEdit(p.id)}>Save</button>
                        <button className="btn btn-secondary" onClick={() => setEditId(null)}>Cancel</button>
                      </td>
                    </>
                  ) : (
                    <>
                      <td>{p.name}</td>
                      <td>${p.price}</td>
                      <td>{p.stockQuantity}</td>
                      <td className="actions">
                        <button className="btn btn-warning" onClick={() => { setEditId(p.id); setEditProduct(p); }}>Edit</button>
                        <button className="btn btn-danger" onClick={() => deleteProduct(p.id)}>Delete</button>
                      </td>
                    </>
                  )}
                </tr>
              ))}
            </tbody>
          </table>

          <h2>Unpaid Orders</h2>
          {orders.filter(o => o.status === 'NEW').length === 0 ? (
            <p>No unpaid orders.</p>
          ) : (
            <table className="table">
              <thead>
                <tr><th>#</th><th>User ID</th><th>Total</th><th>Actions</th></tr>
              </thead>
              <tbody>
                {orders.filter(o => o.status === 'NEW').map(o => (
                  <tr key={o.id}>
                    <td>{o.id}</td>
                    <td>{o.userId}</td>
                    <td>${o.totalPrice}</td>
                    <td className="actions">
                      <button className="btn btn-danger" onClick={() => blacklist(o.userId, 'block')}>Block</button>
                      <button className="btn btn-success" onClick={() => blacklist(o.userId, 'unblock')}>Unblock</button>
                    </td>
                  </tr>
                ))}
              </tbody>
            </table>
          )}
        </>
      )}

      {role !== 'ADMIN' && (
        <>
          <h2>Product Catalog</h2>
          <div className="product-grid">
            {products.map(p => (
              <div key={p.id} className="product-card">
                <h3>{p.name}</h3>
                <p>Price: ${p.price}</p>
                <p className={p.stockQuantity > 0 ? 'in-stock' : 'out-of-stock'}>
                  {p.stockQuantity > 0 ? 'In Stock: ' + p.stockQuantity : 'Out of Stock'}
                </p>
                <button className="btn btn-primary" onClick={() => addToCart(p)} disabled={p.stockQuantity === 0}>
                  Add to Cart
                </button>
              </div>
            ))}
          </div>

          {cart.length > 0 && (
            <div className="cart">
              <h2>Cart</h2>
              {cart.map(i => (
                <p key={i.productId}>{i.name} x {i.quantity} = ${(i.price * i.quantity).toFixed(2)}</p>
              ))}
              <button className="btn btn-primary" onClick={placeOrder}>Place Order</button>
            </div>
          )}

          {orders.filter(o => o.userId === userId && o.status === 'NEW').length > 0 && (
            <div className="pending-orders">
              <h2>My Orders — Pending Payment</h2>
              {orders.filter(o => o.userId === userId && o.status === 'NEW').map(o => (
                <div key={o.id} className="order-row">
                  <span>Order #{o.id} — ${o.totalPrice}</span>
                  <button className="btn btn-success" onClick={() => payOrder(o.id)}>Pay</button>
                </div>
              ))}
            </div>
          )}
        </>
      )}

      {message && <p className="message">{message}</p>}
    </div>
  );
}

export default App;
