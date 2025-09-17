package com.lab1.gift;

import com.lab1.sweets.*;
import org.junit.jupiter.api.BeforeEach;
import org.junit.jupiter.api.Test;
import static org.junit.jupiter.api.Assertions.*;

import java.util.List;

class GiftTest {
    private Gift gift;
    private Sweet chocolate;
    private Sweet candy;
    private Sweet jelly;

    @BeforeEach
    void setUp() {
        gift = new Gift();
        chocolate = new Chocolate("Test Chocolate", 100.0, 500.0, 50.0, "70%", false, Chocolate.ChocolateType.DARK);
        candy = new Candy("Test Candy", 20.0, 400.0, 80.0, "Cherry", true);
        jelly = new Jelly("Test Jelly", 5.0, 300.0, 60.0, "Bear", false);
    }

    @Test
    void testAddSweetAndBasicOperations() {
        assertTrue(gift.isEmpty());
        assertEquals(0, gift.size());
        
        gift.addSweet(chocolate);
        assertFalse(gift.isEmpty());
        assertEquals(1, gift.size());
        assertEquals(100.0, gift.getTotalWeight());
    }

    @Test
    void testAddNullSweetThrowsException() {
        assertThrows(IllegalArgumentException.class, () -> gift.addSweet(null));
    }

    @Test
    void testTotalWeightCalculation() {
        gift.addSweet(chocolate); // 100.0g
        gift.addSweet(candy);     // 20.0g
        gift.addSweet(jelly);     // 5.0g
        
        assertEquals(125.0, gift.getTotalWeight(), 0.01);
    }

    @Test
    void testSortByCalories() {
        gift.addSweet(chocolate); // 500 calories
        gift.addSweet(candy);     // 400 calories
        gift.addSweet(jelly);     // 300 calories
        
        gift.sortByCalories();
        
        List<Sweet> sweets = gift.getSweets();
        assertEquals(300.0, sweets.get(0).getCalories()); // jelly first
        assertEquals(500.0, sweets.get(2).getCalories()); // chocolate last
    }

    @Test
    void testFindBySugarRange() {
        gift.addSweet(chocolate); // sugar: 50.0
        gift.addSweet(candy);     // sugar: 80.0
        gift.addSweet(jelly);     // sugar: 60.0

        List<Sweet> found = gift.findBySugarRange(55.0, 75.0);
        assertEquals(1, found.size());
        assertEquals("Test Jelly", found.get(0).getName());
        
        // test no matches
        found = gift.findBySugarRange(90.0, 95.0);
        assertTrue(found.isEmpty());
    }

    @Test
    void testFindBySugarRangeInvalidInput() {
        assertThrows(IllegalArgumentException.class, () -> gift.findBySugarRange(-1.0, 50.0));
        assertThrows(IllegalArgumentException.class, () -> gift.findBySugarRange(60.0, 50.0));
    }

    @Test
    void testClearGift() {
        gift.addSweet(chocolate);
        gift.addSweet(candy);
        assertEquals(2, gift.size());
        
        gift.clear();
        assertTrue(gift.isEmpty());
        assertEquals(0.0, gift.getTotalWeight());
    }
}