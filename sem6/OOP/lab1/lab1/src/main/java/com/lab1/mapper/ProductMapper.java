package com.lab1.mapper;

import com.lab1.dto.ProductDto;
import com.lab1.model.Product;
import org.mapstruct.Mapper;
import org.mapstruct.factory.Mappers;

// MapStruct mapper — Product -> ProductDto
@Mapper
public interface ProductMapper {
    ProductMapper INSTANCE = Mappers.getMapper(ProductMapper.class);
    ProductDto toDto(Product product);
}