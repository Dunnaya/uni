package com.lab2.mapper;

import com.lab2.dto.OrderResponse;
import com.lab2.model.Order;
import com.lab2.model.OrderItem;
import com.lab2.model.Product;
import com.lab2.model.User;
import java.util.ArrayList;
import java.util.List;
import javax.annotation.processing.Generated;
import org.springframework.stereotype.Component;

@Generated(
    value = "org.mapstruct.ap.MappingProcessor",
    date = "2026-05-19T00:04:09+0300",
    comments = "version: 1.5.5.Final, compiler: javac, environment: Java 24.0.2 (Homebrew)"
)
@Component
public class OrderMapperImpl implements OrderMapper {

    @Override
    public OrderResponse toDto(Order order) {
        if ( order == null ) {
            return null;
        }

        OrderResponse orderResponse = new OrderResponse();

        orderResponse.setUserId( orderUserId( order ) );
        if ( order.getStatus() != null ) {
            orderResponse.setStatus( order.getStatus().name() );
        }
        else {
            orderResponse.setStatus( order.getStatus().name() );
        }
        orderResponse.setId( order.getId() );
        orderResponse.setTotalPrice( order.getTotalPrice() );
        orderResponse.setItems( orderItemListToOrderItemResponseList( order.getItems() ) );

        return orderResponse;
    }

    @Override
    public OrderResponse.OrderItemResponse toItemDto(OrderItem item) {
        if ( item == null ) {
            return null;
        }

        OrderResponse.OrderItemResponse orderItemResponse = new OrderResponse.OrderItemResponse();

        orderItemResponse.setProductId( itemProductId( item ) );
        orderItemResponse.setQuantity( item.getQuantity() );
        orderItemResponse.setPriceAtPurchase( item.getPriceAtPurchase() );

        return orderItemResponse;
    }

    private Long orderUserId(Order order) {
        if ( order == null ) {
            return null;
        }
        User user = order.getUser();
        if ( user == null ) {
            return null;
        }
        Long id = user.getId();
        if ( id == null ) {
            return null;
        }
        return id;
    }

    protected List<OrderResponse.OrderItemResponse> orderItemListToOrderItemResponseList(List<OrderItem> list) {
        if ( list == null ) {
            return null;
        }

        List<OrderResponse.OrderItemResponse> list1 = new ArrayList<OrderResponse.OrderItemResponse>( list.size() );
        for ( OrderItem orderItem : list ) {
            list1.add( toItemDto( orderItem ) );
        }

        return list1;
    }

    private Long itemProductId(OrderItem orderItem) {
        if ( orderItem == null ) {
            return null;
        }
        Product product = orderItem.getProduct();
        if ( product == null ) {
            return null;
        }
        Long id = product.getId();
        if ( id == null ) {
            return null;
        }
        return id;
    }
}
