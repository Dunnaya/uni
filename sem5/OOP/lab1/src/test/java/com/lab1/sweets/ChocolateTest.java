package com.lab1.sweets;

import org.junit.jupiter.api.Test;
import static org.junit.jupiter.api.Assertions.*;

public class ChocolateTest {
    
    @Test
    void testChocolateCreation() {
        Chocolate chocolate = new Chocolate("Roshen Brut", 85.0, 551.0, 18.2, "80%", false, Chocolate.ChocolateType.DARK);

        assertEquals("Roshen Brut", chocolate.getName());
        assertEquals(85.0, chocolate.getWeight());
        assertEquals(551.0, chocolate.getCalories());
        assertEquals(18.2, chocolate.getSugar());
        assertEquals("80%", chocolate.getCocoaPercentage());
        assertFalse(chocolate.hasNuts());
        assertEquals(Chocolate.ChocolateType.DARK, chocolate.getType());
    }

    @Test
    void testChocolateToString() {
        Chocolate chocolate = new Chocolate("Milka Mmmax", 300.0, 553.0, 51.0, "30%", true, Chocolate.ChocolateType.MILK);
        String expected = "Milka Mmmax (Weight: 300.0g, Calories: 553.0 kcal/100g, Sugar: 51.0g), Cocoa: 30%, Contains Nuts: true, Type: MILK";
        assertEquals(expected, chocolate.toString());
    }
}
