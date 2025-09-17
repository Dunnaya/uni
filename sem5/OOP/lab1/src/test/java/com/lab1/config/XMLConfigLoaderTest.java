package com.lab1.config;

import com.lab1.gift.Gift;
import com.lab1.sweets.*;
import org.junit.jupiter.api.Test;
import static org.junit.jupiter.api.Assertions.*;

import java.util.List;

class XMLConfigLoaderTest {
    
    @Test
    void testLoadGiftFromXML() {
        Gift gift = XMLConfigLoader.loadGiftFromXML();
        
        // check gift is not null
        assertNotNull(gift);
        
        // check gift is not empty
        assertFalse(gift.isEmpty());
        
        // check expected number of items
        assertEquals(6, gift.size());
        
        List<Sweet> sweets = gift.getSweets();
        
        // check for specific sweets from XML
        boolean foundMilka = false;
        boolean foundHariboBears = false;
        boolean foundChupaChups = false;
        
        for (Sweet sweet : sweets) {
            if (sweet.getName().equals("Milka")) {
                foundMilka = true;
                assertEquals(100.0, sweet.getWeight());
                assertTrue(sweet instanceof Chocolate);
            } else if (sweet.getName().equals("Haribo Bears")) {
                foundHariboBears = true;
                assertEquals(3.0, sweet.getWeight());
                assertTrue(sweet instanceof Jelly);
            } else if (sweet.getName().equals("Chupa Chups")) {
                foundChupaChups = true;
                assertEquals(12.0, sweet.getWeight());
                assertTrue(sweet instanceof Candy);
            }
        }
        
        assertTrue(foundMilka, "Should contain Milka chocolate from XML");
        assertTrue(foundHariboBears, "Should contain Haribo Bears jelly from XML");
        assertTrue(foundChupaChups, "Should contain Chupa Chups candy from XML");
    }
}