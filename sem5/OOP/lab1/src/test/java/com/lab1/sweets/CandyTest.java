package com.lab1.sweets;

import org.junit.jupiter.api.Test;
import static org.junit.jupiter.api.Assertions.*;

class CandyTest {
    
    @Test
    void testCandySpecificGetters() {
        Candy candy = new Candy("Cow", 15.0, 379.0, 67.8, "Caramel", false);

        assertEquals("Caramel", candy.getFlavor());
        assertFalse(candy.isHardCandy());
    }

    @Test
    void testCandyToString_IncludesAllDetails() {
        Candy candy = new Candy("Chupa Chups", 12.0, 370.0, 91.0, "Sour Apple", true);
        String expected = "Chupa Chups (Weight: 12.0g, Calories: 370.0 kcal/100g, Sugar: 91.0g), Flavor: Sour Apple, Hard Candy: true";
        assertEquals(expected, candy.toString());
    }
}