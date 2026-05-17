import React, { useState, useEffect } from 'react';

const API = 'http://localhost:8080/lab1-1.0-SNAPSHOT/api';

const TOKEN = 'Bearer eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJzdWIiOiJjbGllbnQxIiwicm9sZSI6IkNMSUVOVCJ9.78fx4LHCKQHkxNJP8LaDII0qaL06QRsHuy-680otqrY';

function App() {
  const [products, setProducts] = useState([]);
  const [cart, setCart] = useState([]);
  const [message, setMessage] = useState('');

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
      userId: 2, // client1
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
          setMessage(`Order #${data.id} created. Total: $${data.totalPrice}`);
          setCart([]);
        } else {
          setMessage(`Error: ${data.error}`);
        }
      })
      .catch(() => setMessage('Connection error'));
  };

  return (
    <div style={{ padding: '20px', fontFamily: 'sans-serif', maxWidth: '900px', margin: '0 auto' }}>
      <h1>Store</h1>

      <div style={{ display: 'grid', gridTemplateColumns: 'repeat(3, 1fr)', gap: '15px' }}>
        {products.map(p => (
          <div key={p.id} style={{ border: '1px solid #ccc', padding: '15px', borderRadius: '6px' }}>
            <h3>{p.name}</h3>
            <p>Price: ${p.price}</p>
            <p style={{ color: p.stockQuantity > 0 ? 'green' : 'red' }}>
              {p.stockQuantity > 0 ? `In stock: ${p.stockQuantity}` : 'Out of stock'}
            </p>
            <button
              onClick={() => addToCart(p)}
              disabled={p.stockQuantity === 0}
              style={{ padding: '6px 12px', cursor: 'pointer' }}
            >
              Add to cart
            </button>
          </div>
        ))}
      </div>

      {cart.length > 0 && (
        <div style={{ marginTop: '30px', borderTop: '2px solid #000', paddingTop: '15px' }}>
          <h2>Cart</h2>
          {cart.map(i => (
            <p key={i.productId}>{i.name} × {i.quantity} = ${(i.price * i.quantity).toFixed(2)}</p>
          ))}
          <button
            onClick={placeOrder}
            style={{ padding: '10px 20px', background: '#0070f3', color: '#fff', border: 'none', borderRadius: '4px', cursor: 'pointer' }}
          >
            Place an order
          </button>
        </div>
      )}

      {message && <p style={{ marginTop: '20px', fontWeight: 'bold' }}>{message}</p>}
    </div>
  );
}

export default App;
