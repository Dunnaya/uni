package com.lab2.controller;

import com.lab2.service.AdminService;
import lombok.RequiredArgsConstructor;
import org.springframework.http.ResponseEntity;
import org.springframework.web.bind.annotation.*;

import java.util.Map;

@RestController
@RequestMapping("/api/admin")
@RequiredArgsConstructor
// Spring MVC — replaces lab1's AdminServlet; access restricted to ADMIN in SecurityConfig
public class AdminController {

    private final AdminService adminService;

    // PUT /api/admin/blacklist/{id}?action=unblock
    @PutMapping("/blacklist/{id}")
    public ResponseEntity<Map<String, Boolean>> setBlacklist(
            @PathVariable Long id,
            @RequestParam(defaultValue = "block") String action) {

        boolean blacklisted = !"unblock".equalsIgnoreCase(action);
        adminService.setBlacklisted(id, blacklisted);
        return ResponseEntity.ok(Map.of("blacklisted", blacklisted));
    }
}