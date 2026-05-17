package com.lab1.dto;

import lombok.Data;
import java.util.List;

@Data
public class OrderRequest {
    private int userId;
    private List<OrderItemRequest> items;
}