package com.lab1.dao;

import com.lab1.config.DatabaseConnection;
import com.lab1.dto.OrderItemRequest;
import com.lab1.dto.OrderRequest;
import com.lab1.model.Order;
import org.apache.logging.log4j.LogManager;
import org.apache.logging.log4j.Logger;

import java.sql.*;
import java.util.ArrayList;
import java.util.List;

public class OrderDao {
    private static final Logger logger = LogManager.getLogger(OrderDao.class);

    // checks blacklist, stock, creates order and updates stock_quantity
    public Order createOrder(OrderRequest request) throws SQLException {
        Connection conn = DatabaseConnection.getConnection();

        try (PreparedStatement check = conn.prepareStatement(
                "SELECT is_blacklisted FROM users WHERE id = ?")) {
            check.setInt(1, request.getUserId());
            ResultSet rs = check.executeQuery();
            if (rs.next() && rs.getBoolean("is_blacklisted")) {
                throw new IllegalStateException("User is blacklisted");
            }
        }

        double total = 0;
        List<double[]> prices = new ArrayList<>();
        for (OrderItemRequest item : request.getItems()) {
            try (PreparedStatement ps = conn.prepareStatement(
                    "SELECT price, stock_quantity FROM products WHERE id = ?")) {
                ps.setInt(1, item.getProductId());
                ResultSet rs = ps.executeQuery();
                if (!rs.next()) throw new IllegalArgumentException("Product not found: " + item.getProductId());
                if (rs.getInt("stock_quantity") < item.getQuantity()) {
                    throw new IllegalStateException("Not enough stock for product " + item.getProductId());
                }
                double price = rs.getDouble("price");
                total += price * item.getQuantity();
                prices.add(new double[]{price});
            }
        }

        int orderId;
        try (PreparedStatement ps = conn.prepareStatement(
                "INSERT INTO orders (user_id, status, total_price) VALUES (?, 'NEW', ?) RETURNING id")) {
            ps.setInt(1, request.getUserId());
            ps.setDouble(2, total);
            ResultSet rs = ps.executeQuery();
            rs.next();
            orderId = rs.getInt("id");
        }

        for (int i = 0; i < request.getItems().size(); i++) {
            OrderItemRequest item = request.getItems().get(i);
            try (PreparedStatement ps = conn.prepareStatement(
                    "INSERT INTO order_items (order_id, product_id, quantity, price_at_purchase) VALUES (?,?,?,?)")) {
                ps.setInt(1, orderId);
                ps.setInt(2, item.getProductId());
                ps.setInt(3, item.getQuantity());
                ps.setDouble(4, prices.get(i)[0]);
                ps.executeUpdate();
            }
            try (PreparedStatement ps = conn.prepareStatement(
                    "UPDATE products SET stock_quantity = stock_quantity - ? WHERE id = ?")) {
                ps.setInt(1, item.getQuantity());
                ps.setInt(2, item.getProductId());
                ps.executeUpdate();
            }
        }

        logger.info("Order {} created for user {}, total={}", orderId, request.getUserId(), total);
        return Order.builder().id(orderId).userId(request.getUserId())
                .status("NEW").totalPrice(total).build();
    }

    public boolean payOrder(int orderId) throws SQLException {
        Connection conn = DatabaseConnection.getConnection();
        try (PreparedStatement ps = conn.prepareStatement(
                "UPDATE orders SET status = 'PAID' WHERE id = ? AND status = 'NEW'")) {
            ps.setInt(1, orderId);
            int rows = ps.executeUpdate();
            if (rows > 0) {
                logger.info("Order {} marked as PAID.", orderId);
                return true;
            }
            return false;
        }
    }

    public List<Order> getUnpaidOrders() throws SQLException {
        List<Order> list = new ArrayList<>();
        try (Connection conn = DatabaseConnection.getConnection();
             Statement stmt = conn.createStatement();
             ResultSet rs = stmt.executeQuery("SELECT * FROM orders WHERE status = 'NEW'")) {
            while (rs.next()) {
                list.add(Order.builder()
                        .id(rs.getInt("id"))
                        .userId(rs.getInt("user_id"))
                        .status(rs.getString("status"))
                        .totalPrice(rs.getDouble("total_price"))
                        .build());
            }
        }
        return list;
    }
}