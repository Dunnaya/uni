package com.lab1.sweets;

import org.junit.jupiter.api.Test;
import static org.junit.jupiter.api.Assertions.*;

class JellyTest {
    
    @Test
    void testJellySpecificGetters() {
        Jelly jelly = new Jelly("Gummy Bears", 3.0, 392.0, 59.0, "Bear", false);

        assertEquals("Bear", jelly.getShape());
        assertFalse(jelly.isSourCandy());
    }

    @Test
    void testJellyToString_IncludesAllDetails() {
        Jelly jelly = new Jelly("Citrus Slices", 240.0, 338.0, 75.0, "Slice", true);
        String expected = "Citrus Slices (Weight: 240.0g, Calories: 338.0 kcal/100g, Sugar: 75.0g), Shape: Slice, Sour: true";
        assertEquals(expected, jelly.toString());
    }
}