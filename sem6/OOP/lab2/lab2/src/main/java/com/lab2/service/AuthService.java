package com.lab2.service;

import com.lab2.dto.LoginRequest;
import com.lab2.dto.LoginResponse;
import com.lab2.model.User;
import com.lab2.security.JwtUtil;
import lombok.RequiredArgsConstructor;
import org.springframework.security.authentication.AuthenticationManager;
import org.springframework.security.authentication.UsernamePasswordAuthenticationToken;
import org.springframework.security.core.Authentication;
import org.springframework.stereotype.Service;

@Service
@RequiredArgsConstructor
public class AuthService {

    private final AuthenticationManager authenticationManager;
    private final JwtUtil jwtUtil;

    public LoginResponse login(LoginRequest request) {
        Authentication auth = authenticationManager.authenticate(
                new UsernamePasswordAuthenticationToken(request.getUsername(), request.getPassword())
        );
        User user = (User) auth.getPrincipal();
        String role  = user.getRole();
        String token = jwtUtil.generateToken(user.getUsername(), role);
        return new LoginResponse(token, role, user.getId());
    }
}