package com.lab1.mapper;

import com.lab1.dto.ProductDto;
import com.lab1.model.Product;
import javax.annotation.processing.Generated;

@Generated(
    value = "org.mapstruct.ap.MappingProcessor",
    date = "2026-05-19T00:55:54+0300",
    comments = "version: 1.5.5.Final, compiler: javac, environment: Java 24.0.2 (Homebrew)"
)
public class ProductMapperImpl implements ProductMapper {

    @Override
    public ProductDto toDto(Product product) {
        if ( product == null ) {
            return null;
        }

        ProductDto productDto = new ProductDto();

        productDto.setId( product.getId() );
        productDto.setName( product.getName() );
        productDto.setPrice( product.getPrice() );
        productDto.setStockQuantity( product.getStockQuantity() );

        return productDto;
    }
}
