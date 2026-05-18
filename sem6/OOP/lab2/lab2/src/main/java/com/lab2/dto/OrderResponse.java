package com.lab2.dto;

import lombok.Data;

import java.util.List;

@Data
public class OrderResponse {
    private Long id;
    private Long userId;
    private String status;
    private double totalPrice;
    private List<OrderItemResponse> items;

    @Data
    public static class OrderItemResponse {
        private Long productId;
        private int quantity;
        private double priceAtPurchase;
    }
}