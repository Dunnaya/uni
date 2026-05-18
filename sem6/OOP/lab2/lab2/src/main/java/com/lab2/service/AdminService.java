package com.lab2.service;

import com.lab2.model.User;
import com.lab2.repository.UserRepository;
import lombok.RequiredArgsConstructor;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import org.springframework.stereotype.Service;
import org.springframework.transaction.annotation.Transactional;

@Service
@RequiredArgsConstructor
public class AdminService {

    private static final Logger log = LoggerFactory.getLogger(AdminService.class);

    private final UserRepository userRepository;

    @Transactional
    public void setBlacklisted(Long userId, boolean blacklisted) {
        User user = userRepository.findById(userId)
                .orElseThrow(() -> new IllegalArgumentException("User not found: " + userId));
        user.setBlacklisted(blacklisted);
        log.info("User {} blacklisted={}", userId, blacklisted);
    }
}