package com.lab1.model;

import lombok.AllArgsConstructor;
import lombok.Builder;
import lombok.Data;
import lombok.NoArgsConstructor;

@Data
@Builder
@NoArgsConstructor
@AllArgsConstructor
// builder via lombok
public class User {
    private int id;
    private String username;
    private String role;
    private boolean blacklisted;
}