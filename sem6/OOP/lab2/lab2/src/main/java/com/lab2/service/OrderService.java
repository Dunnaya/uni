package com.lab2.service;

import com.lab2.dto.OrderItemRequest;
import com.lab2.dto.OrderRequest;
import com.lab2.dto.OrderResponse;
import com.lab2.mapper.OrderMapper;
import com.lab2.model.*;
import com.lab2.repository.OrderRepository;
import com.lab2.repository.ProductRepository;
import com.lab2.repository.UserRepository;
import lombok.RequiredArgsConstructor;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import org.springframework.stereotype.Service;
import org.springframework.transaction.annotation.Transactional;

import java.util.List;
import java.util.stream.Collectors;

@Service
@RequiredArgsConstructor
public class OrderService {

    private static final Logger log = LoggerFactory.getLogger(OrderService.class);

    private final OrderRepository    orderRepository;
    private final ProductRepository  productRepository;
    private final UserRepository     userRepository;
    private final OrderMapper        orderMapper;

    // @Transactional — replaces lab1's manual connection + rollback logic
    @Transactional
    public OrderResponse createOrder(OrderRequest request) {
        User user = userRepository.findById(request.getUserId())
                .orElseThrow(() -> new IllegalArgumentException("User not found: " + request.getUserId()));

        if (user.isBlacklisted()) {
            throw new IllegalStateException("User is blacklisted");
        }

        Order order = Order.builder()
                .user(user)
                .status(OrderStatus.NEW)
                .totalPrice(0)
                .build();

        double total = 0;
        for (OrderItemRequest itemReq : request.getItems()) {
            Product product = productRepository.findById(itemReq.getProductId())
                    .orElseThrow(() -> new IllegalArgumentException("Product not found: " + itemReq.getProductId()));

            if (product.getStockQuantity() < itemReq.getQuantity()) {
                throw new IllegalStateException("Not enough stock for product: " + product.getName());
            }

            OrderItem item = OrderItem.builder()
                    .order(order)
                    .product(product)
                    .quantity(itemReq.getQuantity())
                    .priceAtPurchase(product.getPrice())
                    .build();

            order.getItems().add(item);
            total += product.getPrice() * itemReq.getQuantity();

            // Hibernate tracks the entity — no raw SQL UPDATE needed
            product.setStockQuantity(product.getStockQuantity() - itemReq.getQuantity());
        }

        order.setTotalPrice(total);
        Order saved = orderRepository.save(order);
        log.info("Order {} created for user {}, total={}", saved.getId(), user.getId(), total);
        return orderMapper.toDto(saved);
    }

    @Transactional
    public OrderResponse payOrder(Long orderId) {
        Order order = orderRepository.findById(orderId)
                .orElseThrow(() -> new IllegalArgumentException("Order not found: " + orderId));

        if (order.getStatus() != OrderStatus.NEW) {
            throw new IllegalStateException("Order is already paid");
        }

        order.setStatus(OrderStatus.PAID);
        log.info("Order {} marked as PAID", orderId);
        return orderMapper.toDto(orderRepository.save(order));
    }

    @Transactional(readOnly = true)
    public List<OrderResponse> getUnpaidOrders() {
        return orderRepository.findAllByStatus(OrderStatus.NEW)
                .stream()
                .map(orderMapper::toDto)
                .collect(Collectors.toList());
    }
}