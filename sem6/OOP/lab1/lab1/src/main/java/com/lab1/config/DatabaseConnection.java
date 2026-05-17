package com.lab1.config;

import java.sql.Connection;
import java.sql.DriverManager;
import java.sql.SQLException;
import org.apache.logging.log4j.LogManager;
import org.apache.logging.log4j.Logger;

// singleton
public class DatabaseConnection {
    private static final Logger logger = LogManager.getLogger(DatabaseConnection.class);

    private static final String URL  = "jdbc:postgresql://localhost:5432/lab1";
    private static final String USER = "postgres";
    private static final String PASS = "37860943";

    private static volatile Connection connection;

    private DatabaseConnection() {}

    public static Connection getConnection() {
        if (connection == null) {
            synchronized (DatabaseConnection.class) {
                if (connection == null) {
                    try {
                        Class.forName("org.postgresql.Driver");
                        connection = DriverManager.getConnection(URL, USER, PASS);
                        logger.info("JDBC Connection established.");
                    } catch (Exception e) {
                        logger.error("DB Connection Error", e);
                    }
                }
            }
        }
        try {
            if (connection != null && connection.isClosed()) {
                connection = DriverManager.getConnection(URL, USER, PASS);
            }
        } catch (SQLException e) {
            logger.error("DB reconnect error", e);
        }
        return connection;
    }
}