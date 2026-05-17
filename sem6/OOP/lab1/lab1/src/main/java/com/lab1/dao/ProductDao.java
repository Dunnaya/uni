package com.lab1.dao;

import com.lab1.config.DatabaseConnection;
import com.lab1.model.Product;
import org.apache.logging.log4j.LogManager;
import org.apache.logging.log4j.Logger;

import java.sql.*;
import java.util.ArrayList;
import java.util.List;

public class ProductDao {
    private static final Logger logger = LogManager.getLogger(ProductDao.class);

    public List<Product> getAllProducts() {
        List<Product> list = new ArrayList<>();
        try (Connection conn = DatabaseConnection.getConnection();
             Statement stmt = conn.createStatement();
             ResultSet rs = stmt.executeQuery("SELECT * FROM products")) {

            while (rs.next()) {
                list.add(Product.builder()
                        .id(rs.getInt("id"))
                        .name(rs.getString("name"))
                        .price(rs.getDouble("price"))
                        .stockQuantity(rs.getInt("stock_quantity"))
                        .build());
            }
            logger.info("Fetched {} products.", list.size());
        } catch (SQLException e) {
            logger.error("Error fetching products", e);
        }
        return list;
    }
}