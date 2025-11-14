package com.lab1.gift;

import com.lab1.sweets.*;
import org.junit.jupiter.api.BeforeEach;
import org.junit.jupiter.api.Test;
import static org.junit.jupiter.api.Assertions.*;

import java.util.List;

class GiftTest {
    private Gift gift;
    // meaningful names to easily track sorting order
    private Sweet lowCalorie;
    private Sweet mediumCalorie;
    private Sweet highCalorie;

    @BeforeEach
    void setUp() {
        gift = new Gift();
        // 300 calories (Low Sugar: 60)
        lowCalorie = new Jelly("Jelly", 5.0, 300.0, 60.0, "Bear", false);
        // 400 calories (High Sugar: 80)
        mediumCalorie = new Candy("Candy", 20.0, 400.0, 80.0, "Cherry", true);
        // 500 calories (Medium Sugar: 50)
        highCalorie = new Chocolate("Chocolate", 100.0, 500.0, 50.0, "70%", false, Chocolate.ChocolateType.DARK);
    }

    @Test
    void testBasicOperations_Add_Size_Empty_Weight() {
        // testing Add, Size, Empty, Weight all at once
        assertTrue(gift.isEmpty());
        assertEquals(0, gift.size());
        
        gift.addSweet(highCalorie); // 100.0g
        gift.addSweet(mediumCalorie); // 20.0g
        
        assertFalse(gift.isEmpty());
        assertEquals(2, gift.size());
        assertEquals(120.0, gift.getTotalWeight(), 0.01);
    }

    @Test
    void testAddNullSweetThrowsException() {
        assertThrows(IllegalArgumentException.class, () -> gift.addSweet(null));
    }

    @Test
    void testSortByCalories_AscendingOrder() {
        gift.addSweet(highCalorie);
        gift.addSweet(mediumCalorie);
        gift.addSweet(lowCalorie);
        
        gift.sortByCalories();
        
        List<Sweet> sweets = gift.getSweets();
        // expected order: 300 (low) -> 400 (medium) -> 500 (high)
        assertEquals(lowCalorie.getName(), sweets.get(0).getName());
        assertEquals(mediumCalorie.getName(), sweets.get(1).getName());
        assertEquals(highCalorie.getName(), sweets.get(2).getName());
    }

    @Test
    void testFindBySugarRange_SingleMatchAndEmptySet() {
        gift.addSweet(highCalorie);     // sugar: 50.0
        gift.addSweet(mediumCalorie);   // sugar: 80.0
        gift.addSweet(lowCalorie);      // sugar: 60.0

        // 55.0 - 75.0 diapazon (= lowCalorie: 60.0)
        List<Sweet> found = gift.findBySugarRange(55.0, 75.0);
        assertEquals(1, found.size());
        assertEquals(lowCalorie, found.get(0));
        
        // with no matches
        found = gift.findBySugarRange(90.0, 95.0);
        assertTrue(found.isEmpty());
    }
    
    @Test
    void testFindBySugarRange_FindsAll() {
        gift.addSweet(highCalorie);     
        gift.addSweet(mediumCalorie);   
        gift.addSweet(lowCalorie);      
        
        // 40.0 - 100.0 diapazon 
        List<Sweet> found = gift.findBySugarRange(40.0, 100.0);
        assertEquals(3, found.size());
    }

    @Test
    void testFindBySugarRange_InvalidInputThrowsException() {
        // minSugar < 0
        assertThrows(IllegalArgumentException.class, () -> gift.findBySugarRange(-1.0, 50.0));
        // minSugar > maxSugar
        assertThrows(IllegalArgumentException.class, () -> gift.findBySugarRange(60.0, 50.0));
    }

    @Test
    void testClearGift() {
        gift.addSweet(highCalorie);
        gift.addSweet(lowCalorie);
        
        gift.clear();
        assertTrue(gift.isEmpty());
        assertEquals(0.0, gift.getTotalWeight());
        assertEquals(0, gift.size());
    }
}