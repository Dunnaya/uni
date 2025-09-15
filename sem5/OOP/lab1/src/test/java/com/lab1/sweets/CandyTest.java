package com.lab1.sweets;

import org.junit.jupiter.api.Test;
import static org.junit.jupiter.api.Assertions.*;

class CandyTest {
    
    @Test
    void testCandyCreation() {
        Candy candy = new Candy("Cow", 15.0, 57.0, 11.0, "Caramel", false);

        assertEquals("Cow", candy.getName());
        assertEquals(15.0, candy.getWeight());
        assertEquals(57.0, candy.getCalories());
        assertEquals(11.0, candy.getSugar());
        assertEquals("Caramel", candy.getFlavor());
        assertFalse(candy.isHardCandy());
    }

    @Test
    void testCandyToString() {
        Candy candy = new Candy("Chupa Chups", 12.0, 44.5, 11.0, "Sour Apple", true);
        String expected = "Chupa Chups (Weight: 12.0g, Calories: 44.5kcal, Sugar: 11.0g), Flavor: Sour Apple, Hard Candy: true";
        assertEquals(expected, candy.toString());
    }
}
