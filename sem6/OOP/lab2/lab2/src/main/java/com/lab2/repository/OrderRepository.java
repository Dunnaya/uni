package com.lab2.repository;

import com.lab2.model.Order;
import com.lab2.model.OrderStatus;
import org.springframework.data.jpa.repository.JpaRepository;

import java.util.List;

public interface OrderRepository extends JpaRepository<Order, Long> {
    // Spring Data derives query from method name — no SQL needed
    List<Order> findAllByStatus(OrderStatus status);
}