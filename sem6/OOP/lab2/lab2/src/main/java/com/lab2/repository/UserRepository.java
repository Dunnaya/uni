package com.lab2.repository;

import com.lab2.model.User;
import org.springframework.data.jpa.repository.JpaRepository;

import java.util.Optional;

// Spring Data JPA — no boilerplate SQL needed
public interface UserRepository extends JpaRepository<User, Long> {
    Optional<User> findByUsername(String username);
}