package com.lab2.mapper;

import com.lab2.dto.OrderResponse;
import com.lab2.model.Order;
import com.lab2.model.OrderItem;
import org.mapstruct.Mapper;
import org.mapstruct.Mapping;

@Mapper(componentModel = "spring")
public interface OrderMapper {

    @Mapping(source = "user.id", target = "userId")
    @Mapping(source = "status",  target = "status", defaultExpression = "java(order.getStatus().name())")
    OrderResponse toDto(Order order);

    @Mapping(source = "product.id", target = "productId")
    OrderResponse.OrderItemResponse toItemDto(OrderItem item);
}