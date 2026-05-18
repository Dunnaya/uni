package com.lab2.controller;

import com.lab2.dto.OrderRequest;
import com.lab2.dto.OrderResponse;
import com.lab2.service.OrderService;
import lombok.RequiredArgsConstructor;
import org.springframework.http.HttpStatus;
import org.springframework.http.ResponseEntity;
import org.springframework.web.bind.annotation.*;

import java.util.List;

@RestController
@RequestMapping("/api/orders")
@RequiredArgsConstructor
// Spring MVC — replaces lab1's OrderServlet
public class OrderController {

    private final OrderService orderService;

    // POST /api/orders — create order (any authenticated user)
    @PostMapping
    public ResponseEntity<OrderResponse> create(@RequestBody OrderRequest request) {
        return ResponseEntity.status(HttpStatus.CREATED).body(orderService.createOrder(request));
    }

    // PUT /api/orders/{id} — pay order
    @PutMapping("/{id}")
    public ResponseEntity<OrderResponse> pay(@PathVariable Long id) {
        return ResponseEntity.ok(orderService.payOrder(id));
    }

    // GET /api/orders — list unpaid orders (ADMIN only, secured via SecurityConfig)
    @GetMapping
    public ResponseEntity<List<OrderResponse>> getUnpaid() {
        return ResponseEntity.ok(orderService.getUnpaidOrders());
    }
}