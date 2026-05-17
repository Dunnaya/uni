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
public class Product {
    private int id;
    private String name;
    private double price;
    private int stockQuantity;
}