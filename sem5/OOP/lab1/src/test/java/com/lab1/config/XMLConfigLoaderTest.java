package com.lab1.config;

import com.lab1.gift.Gift;
import com.lab1.sweets.*;
import org.junit.jupiter.api.Test;
import static org.junit.jupiter.api.Assertions.*;

import java.util.List;

class XMLConfigLoaderTest {
    
    @Test
    void testLoadGiftFromXML_BasicChecks() {
        Gift gift = XMLConfigLoader.loadGiftFromXML();
        
        assertNotNull(gift, "Gift should be initialized.");
        assertFalse(gift.isEmpty(), "Gift should not be empty if XML found.");
        assertEquals(6, gift.size(), "Should contain 6 sweets from gift.xml.");
        
        double expectedTotalWeight = 100.0 + 85.0 + 12.0 + 15.0 + 3.0 + 5.0; // sum of weights from gift.xml
        assertEquals(expectedTotalWeight, gift.getTotalWeight(), 0.01, "Total weight should match XML data.");
    }
    
    // check Chocolate parsing
    @Test
    void testLoadGiftFromXML_ChocolateDataIntegrity() {
        Gift gift = XMLConfigLoader.loadGiftFromXML();
        List<Sweet> chocolates = gift.getSweets().stream()
                .filter(s -> s.getName().equals("Roshen Dark"))
                .toList();

        assertEquals(1, chocolates.size());
        Chocolate darkChoco = (Chocolate) chocolates.get(0);
        
        assertEquals(85.0, darkChoco.getWeight());
        assertEquals(18.2, darkChoco.getSugar());
        assertEquals(Chocolate.ChocolateType.DARK, darkChoco.getType());
        assertTrue(darkChoco.hasNuts()); // Roshen Dark has true in XML
    }

    // check Candy parsing
    @Test
    void testLoadGiftFromXML_CandyDataIntegrity() {
        Gift gift = XMLConfigLoader.loadGiftFromXML();
        List<Sweet> candies = gift.getSweets().stream()
                .filter(s -> s.getName().equals("Cow"))
                .toList();
        
        assertEquals(1, candies.size());
        Candy cowCandy = (Candy) candies.get(0);
        
        assertEquals(15.0, cowCandy.getWeight());
        assertEquals("Caramel", cowCandy.getFlavor());
        assertFalse(cowCandy.isHardCandy()); // Cow has false in XML
    }
    
    // check Jelly parsing
    @Test
    void testLoadGiftFromXML_JellyDataIntegrity() {
        Gift gift = XMLConfigLoader.loadGiftFromXML();
        List<Sweet> jellies = gift.getSweets().stream()
                .filter(s -> s.getName().equals("Sour Worms"))
                .toList();
        
        assertEquals(1, jellies.size());
        Jelly sourWorms = (Jelly) jellies.get(0);
        
        assertEquals(5.0, sourWorms.getWeight());
        assertEquals("Worm", sourWorms.getShape());
        assertTrue(sourWorms.isSourCandy()); // Sour Worms has true in XML
    }
}