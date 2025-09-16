package com.lab1.gift;

import com.lab1.sweets.*;
import org.junit.jupiter.api.BeforeEach;
import org.junit.jupiter.api.Test;
import static org.junit.jupiter.api.Assertions.*;

import java.io.ByteArrayOutputStream;
import java.io.PrintStream;
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
    void testGiftCreation() {
        assertNotNull(gift);
        assertTrue(gift.isEmpty());
        assertEquals(0, gift.size());
    }

    @Test
    void testAddSweet() {
        gift.addSweet(chocolate);
        assertEquals(1, gift.size());
        assertFalse(gift.isEmpty());
    }

    @Test
    void testAddNullSweet() {
        assertThrows(IllegalArgumentException.class, () -> gift.addSweet(null));
    }

    @Test
    void testAddMultipleSweets() {
        gift.addSweet(chocolate);
        gift.addSweet(candy);
        gift.addSweet(jelly);
        
        assertEquals(3, gift.size());
        List<Sweet> sweets = gift.getSweets();
        assertEquals(3, sweets.size());
    }

    @Test
    void testGetTotalWeight() {
        assertEquals(0.0, gift.getTotalWeight(), 0.01);
        
        gift.addSweet(chocolate); // 100.0g
        assertEquals(100.0, gift.getTotalWeight(), 0.01);
        
        gift.addSweet(candy); // 20.0g
        assertEquals(120.0, gift.getTotalWeight(), 0.01);
        
        gift.addSweet(jelly); // 5.0g
        assertEquals(125.0, gift.getTotalWeight(), 0.01);
    }

    @Test
    void testSortByCalories() {
        // order is: chocolate(500), candy(400), jelly(300)
        gift.addSweet(chocolate);
        gift.addSweet(candy);
        gift.addSweet(jelly);
        
        gift.sortByCalories();
        
        List<Sweet> sweets = gift.getSweets();
        // after sorting: jelly(300), candy(400), chocolate(500)
        assertEquals(300.0, sweets.get(0).getCalories(), 0.01);
        assertEquals(400.0, sweets.get(1).getCalories(), 0.01);
        assertEquals(500.0, sweets.get(2).getCalories(), 0.01);
    }

    @Test
    void testFindBySugarRange() {
        gift.addSweet(chocolate); // sugar: 50.0
        gift.addSweet(candy);     // sugar: 80.0
        gift.addSweet(jelly);     // sugar: 60.0

        // find by sugar from 55 to 75
        List<Sweet> found = gift.findBySugarRange(55.0, 75.0);
        assertEquals(1, found.size());
        assertEquals("Test Jelly", found.get(0).getName());

        // find all sweets
        found = gift.findBySugarRange(0.0, 100.0);
        assertEquals(3, found.size());

        // find nothing
        found = gift.findBySugarRange(90.0, 95.0);
        assertEquals(0, found.size());
    }

    @Test
    void testFindBySugarRangeInvalidInput() {
        assertThrows(IllegalArgumentException.class, () -> gift.findBySugarRange(-1.0, 50.0));
        assertThrows(IllegalArgumentException.class, () -> gift.findBySugarRange(50.0, -1.0));
        assertThrows(IllegalArgumentException.class, () -> gift.findBySugarRange(60.0, 50.0));
    }

    @Test
    void testShowGiftEmpty() {
        ByteArrayOutputStream outContent = new ByteArrayOutputStream();
        System.setOut(new PrintStream(outContent));
        
        gift.showGift();
        
        assertTrue(outContent.toString().contains("The gift is empty"));
        System.setOut(System.out); // standard output restoration
    }

    @Test
    void testShowGiftWithSweets() {
        gift.addSweet(chocolate);
        gift.addSweet(candy);
        
        ByteArrayOutputStream outContent = new ByteArrayOutputStream();
        System.setOut(new PrintStream(outContent));
        
        gift.showGift();
        
        String output = outContent.toString();
        assertTrue(output.contains("Test Chocolate"));
        assertTrue(output.contains("Test Candy"));
        assertTrue(output.contains("1."));
        assertTrue(output.contains("2."));
        
        System.setOut(System.out); // standard output restoration
    }

    @Test
    void testGetSweetsReturnsACopy() {
        gift.addSweet(chocolate);
        List<Sweet> sweets1 = gift.getSweets();
        List<Sweet> sweets2 = gift.getSweets();

        // check that these are different objects
        assertNotSame(sweets1, sweets2);

        // but they have the same content
        assertEquals(sweets1.size(), sweets2.size());
        assertEquals(sweets1.get(0), sweets2.get(0));
    }

    @Test
    void testClearGift() {
        gift.addSweet(chocolate);
        gift.addSweet(candy);
        assertEquals(2, gift.size());
        
        gift.clear();
        assertEquals(0, gift.size());
        assertTrue(gift.isEmpty());
        assertEquals(0.0, gift.getTotalWeight(), 0.01);
    }

    @Test
    void testEmptyGiftOperations() {
        assertEquals(0.0, gift.getTotalWeight(), 0.01);
        
        List<Sweet> found = gift.findBySugarRange(0.0, 100.0);
        assertTrue(found.isEmpty());
        
        // empty gift sorting should not throw exception
        assertDoesNotThrow(() -> gift.sortByCalories());
    }
}