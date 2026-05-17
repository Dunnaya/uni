package com.lab1.model;

import lombok.AllArgsConstructor;
import lombok.Builder;
import lombok.Data;
import lombok.NoArgsConstructor;

@Data
@Builder
@NoArgsConstructor
@AllArgsConstructor
// builder via lombok
public class OrderItem {
    private int id;
    private int orderId;
    private int productId;
    private int quantity;
    private double priceAtPurchase;
}