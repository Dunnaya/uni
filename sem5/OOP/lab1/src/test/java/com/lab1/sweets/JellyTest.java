package com.lab1.sweets;

import org.junit.jupiter.api.Test;
import static org.junit.jupiter.api.Assertions.*;

public class JellyTest {
    
    @Test
    void testJellyCreation() {
        Jelly jelly = new Jelly("Gummy Bears", 3.0, 392.0, 59.0, "Bear", false);

        assertEquals("Gummy Bears", jelly.getName());
        assertEquals(3.0, jelly.getWeight());
        assertEquals(392.0, jelly.getCalories());
        assertEquals(59.0, jelly.getSugar());
        assertEquals("Bear", jelly.getShape());
        assertFalse(jelly.isSourCandy());
    }

    @Test
    void testJellyToString() {
        Jelly jelly = new Jelly("Citrus Slices", 240.0, 338.0, 75.0, "Slice", true);
        String expected = "Citrus Slices (Weight: 240.0g, Calories: 338.0 kcal/100g, Sugar: 75.0g), Shape: Slice, Sour: true";
        assertEquals(expected, jelly.toString());
    }
}
