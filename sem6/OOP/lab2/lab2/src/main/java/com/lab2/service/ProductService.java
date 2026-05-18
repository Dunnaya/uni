package com.lab2.service;

import com.lab2.dto.ProductDto;
import com.lab2.mapper.ProductMapper;
import com.lab2.model.Product;
import com.lab2.repository.ProductRepository;
import lombok.RequiredArgsConstructor;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import org.springframework.stereotype.Service;
import org.springframework.transaction.annotation.Transactional;

import java.util.List;
import java.util.stream.Collectors;

@Service
@RequiredArgsConstructor
public class ProductService {

    private static final Logger log = LoggerFactory.getLogger(ProductService.class);

    private final ProductRepository productRepository;
    private final ProductMapper productMapper;

    public List<ProductDto> getAllProducts() {
        List<ProductDto> products = productRepository.findAll()
                .stream()
                .map(productMapper::toDto)
                .collect(Collectors.toList());
        log.info("Fetched {} products", products.size());
        return products;
    }

    @Transactional
    public ProductDto addProduct(ProductDto dto) {
        Product saved = productRepository.save(productMapper.toEntity(dto));
        log.info("Product added: {}", saved.getId());
        return productMapper.toDto(saved);
    }

    @Transactional
    public ProductDto updateProduct(Long id, ProductDto dto) {
        Product product = productRepository.findById(id)
                .orElseThrow(() -> new IllegalArgumentException("Product not found: " + id));
        product.setName(dto.getName());
        product.setPrice(dto.getPrice());
        product.setStockQuantity(dto.getStockQuantity());
        log.info("Product updated: {}", id);
        return productMapper.toDto(productRepository.save(product));
    }

    @Transactional
    public void deleteProduct(Long id) {
        productRepository.deleteById(id);
        log.info("Product deleted: {}", id);
    }
}