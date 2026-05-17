package com.lab1.model;

import lombok.AllArgsConstructor;
import lombok.Builder;
import lombok.Data;
import lombok.NoArgsConstructor;
import java.util.List;

@Data
@Builder
@NoArgsConstructor
@AllArgsConstructor
// builder via lombok
public class Order {
    private int id;
    private int userId;
    private String status;
    private double totalPrice;
    private List<OrderItem> items;
}