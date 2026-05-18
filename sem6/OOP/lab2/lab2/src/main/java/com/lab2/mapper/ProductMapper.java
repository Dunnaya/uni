package com.lab2.mapper;

import com.lab2.dto.ProductDto;
import com.lab2.model.Product;
import org.mapstruct.Mapper;
import org.mapstruct.factory.Mappers;

// MapStruct generates implementation at compile time
@Mapper(componentModel = "spring")
public interface ProductMapper {
    ProductMapper INSTANCE = Mappers.getMapper(ProductMapper.class);

    ProductDto toDto(Product product);
    Product toEntity(ProductDto dto);
}