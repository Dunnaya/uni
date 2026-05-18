package com.lab2.security;

import com.auth0.jwt.JWT;
import com.auth0.jwt.algorithms.Algorithm;
import com.auth0.jwt.exceptions.JWTVerificationException;
import com.auth0.jwt.interfaces.DecodedJWT;
import org.springframework.beans.factory.annotation.Value;
import org.springframework.stereotype.Component;

import java.util.Date;

// Auth0 java-jwt wrapper — matches approach used in lab1
@Component
public class JwtUtil {

    @Value("${jwt.secret}")
    private String secret;

    @Value("${jwt.expiration-ms}")
    private long expirationMs;

    public String generateToken(String username, String role) {
        return JWT.create()
                .withSubject(username)
                .withClaim("role", role)
                .withIssuedAt(new Date())
                .withExpiresAt(new Date(System.currentTimeMillis() + expirationMs))
                .sign(Algorithm.HMAC256(secret));
    }

    public DecodedJWT verify(String token) throws JWTVerificationException {
        return JWT.require(Algorithm.HMAC256(secret)).build().verify(token);
    }

    public String extractUsername(String token) {
        return verify(token).getSubject();
    }

    public String extractRole(String token) {
        return verify(token).getClaim("role").asString();
    }
}