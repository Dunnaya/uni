package com.lab1.controller;

import com.google.gson.Gson;
import com.lab1.dao.ProductDao;
import com.lab1.dto.ProductDto;
import com.lab1.mapper.ProductMapper;
import com.lab1.model.Product;
import jakarta.servlet.annotation.WebServlet;
import jakarta.servlet.http.HttpServlet;
import jakarta.servlet.http.HttpServletRequest;
import jakarta.servlet.http.HttpServletResponse;

import java.io.IOException;
import java.util.List;
import java.util.stream.Collectors;

@WebServlet("/api/products")
// MVC controller - REST endpoint for products
public class ProductServlet extends HttpServlet {
    private final ProductDao productDao = new ProductDao();
    private final Gson gson = new Gson();

    @Override
    protected void doGet(HttpServletRequest req, HttpServletResponse resp) throws IOException {
        resp.setHeader("Access-Control-Allow-Origin", "*");
        resp.setContentType("application/json");
        resp.setCharacterEncoding("UTF-8");

        List<Product> products = productDao.getAllProducts();
        List<ProductDto> dtos = products.stream()
                .map(ProductMapper.INSTANCE::toDto)
                .collect(Collectors.toList());

        resp.getWriter().write(gson.toJson(dtos));
    }
}