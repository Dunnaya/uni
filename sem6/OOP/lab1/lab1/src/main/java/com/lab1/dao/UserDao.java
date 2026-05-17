package com.lab1.dao;

import com.lab1.config.DatabaseConnection;
import org.apache.logging.log4j.LogManager;
import org.apache.logging.log4j.Logger;

import java.sql.*;

public class UserDao {
    private static final Logger logger = LogManager.getLogger(UserDao.class);

    public boolean setBlacklisted(int userId, boolean blacklisted) throws SQLException {
        try (Connection conn = DatabaseConnection.getConnection();
             PreparedStatement ps = conn.prepareStatement(
                     "UPDATE users SET is_blacklisted = ? WHERE id = ?")) {
            ps.setBoolean(1, blacklisted);
            ps.setInt(2, userId);
            int rows = ps.executeUpdate();
            logger.info("User {} blacklisted={}", userId, blacklisted);
            return rows > 0;
        }
    }
}