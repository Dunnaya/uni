// ! might partly replace other tests

package com.lab1.sweets;

import org.junit.jupiter.api.Test;
import static org.junit.jupiter.api.Assertions.*;

class SweetTest {
    
    // "test" class to test abstract Sweet class
    private static class TestSweet extends Sweet {
        public TestSweet(String name, double weight, double calories, double sugar) {
            super(name, weight, calories, sugar);
        }
    }

    @Test
    void testSweetCreation() {
        Sweet sweet = new TestSweet("Test Sweet", 10.0, 300.0, 45.0);
        
        assertEquals("Test Sweet", sweet.getName());
        assertEquals(10.0, sweet.getWeight(), 0.01);
        assertEquals(300.0, sweet.getCalories(), 0.01);
        assertEquals(45.0, sweet.getSugar(), 0.01);
    }

    @Test
    void testSweetToString() {
        Sweet sweet = new TestSweet("Test Sweet", 10.0, 300.0, 45.0);
        String expected = "Test Sweet (Weight: 10.0g, Calories: 300.0 kcal/100g, Sugar: 45.0g)";
        assertEquals(expected, sweet.toString());
    }

    @Test
    void testSweetWithZeroValues() {
        Sweet sweet = new TestSweet("Zero Sweet", 0.0, 0.0, 0.0);
        
        assertEquals("Zero Sweet", sweet.getName());
        assertEquals(0.0, sweet.getWeight(), 0.01);
        assertEquals(0.0, sweet.getCalories(), 0.01);
        assertEquals(0.0, sweet.getSugar(), 0.01);
    }

    @Test
    void testSweetWithDecimalValues() {
        Sweet sweet = new TestSweet("Decimal Sweet", 12.5, 342.7, 67.3);
        
        assertEquals(12.5, sweet.getWeight(), 0.01);
        assertEquals(342.7, sweet.getCalories(), 0.01);
        assertEquals(67.3, sweet.getSugar(), 0.01);
    }
}