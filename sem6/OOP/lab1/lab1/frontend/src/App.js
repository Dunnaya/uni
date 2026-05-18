import React, { useState, useEffect } from 'react';
import './App.css';

const API = 'http://localhost:8080/lab1-1.0-SNAPSHOT/api';

const TOKEN = 'Bearer eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJzdWIiOiJjbGllbnQxIiwicm9sZSI6IkNMSUVOVCJ9.78fx4LHCKQHkxNJP8LaDII0qaL06QRsHuy-680otqrY';

function App() {
  const [products, setProducts] = useState([]);
  const [cart, setCart]         = useState([]);
  const [message, setMessage]   = useState('');

  useEffect(() => {
    fetch(`${API}/products`)
      .then(res => res.json())
      .then(data => setProducts(data))
      .catch(err => console.error('Download error:', err));
  }, []);

  const addToCart = (product) => {
    setCart(prev => {
      const existing = prev.find(i => i.productId === product.id);
      if (existing) {
        return prev.map(i => i.productId === product.id
          ? { ...i, quantity: i.quantity + 1 } : i);
      }
      return [...prev, { productId: product.id, quantity: 1, name: product.name, price: product.price }];
    });
  };

  const placeOrder = () => {
    if (cart.length === 0) return;

    const orderRequest = {
      userId: 2,
      items: cart.map(i => ({ productId: i.productId, quantity: i.quantity }))
    };

    fetch(`${API}/orders`, {
      method: 'POST',
      headers: { 'Content-Type': 'application/json', 'Authorization': TOKEN },
      body: JSON.stringify(orderRequest)
    })
      .then(res => res.json())
      .then(data => {
        if (data.id) {
          setMessage('Order #' + data.id + ' created. Total: $' + data.totalPrice);
          setCart([]);
        } else {
          setMessage('Error: ' + data.error);
        }
      })
      .catch(() => setMessage('Connection error'));
  };

  return (
    <div className="app">
      <h1 className="app-title">Shop</h1>

      <h2>Product Catalog</h2>
      <div className="product-grid">
        {products.map(p => (
          <div key={p.id} className="product-card">
            <h3>{p.name}</h3>
            <p>Price: ${p.price}</p>
            <p className={p.stockQuantity > 0 ? 'in-stock' : 'out-of-stock'}>
              {p.stockQuantity > 0 ? 'In Stock: ' + p.stockQuantity : 'Out of Stock'}
            </p>
            <button
              className="btn btn-primary"
              onClick={() => addToCart(p)}
              disabled={p.stockQuantity === 0}
            >
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
          <button className="btn btn-primary" onClick={placeOrder}>
            Place Order
          </button>
        </div>
      )}

      {message && <p className="message">{message}</p>}
    </div>
  );
}

export default App;
