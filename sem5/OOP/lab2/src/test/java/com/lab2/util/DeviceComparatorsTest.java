package com.lab2.util;

import com.lab2.model.Device;
import org.junit.jupiter.api.BeforeEach;
import org.junit.jupiter.api.Test;
import java.util.Arrays;
import java.util.Collections;
import java.util.List;
import static org.junit.jupiter.api.Assertions.*;

class DeviceComparatorsTest {
    private List<Device> devices;
    
    @BeforeEach
    void setUp() {
        Device.ComponentType type1 = new Device.ComponentType(true, 50, false, 
            Device.ComponentGroup.INPUT_OUTPUT, Arrays.asList(Device.Port.USB));
        Device.ComponentType type2 = new Device.ComponentType(false, 100, true, 
            Device.ComponentGroup.PROCESSING, Arrays.asList(Device.Port.LGA1700));
        Device.ComponentType type3 = new Device.ComponentType(false, 25, false, 
            Device.ComponentGroup.MEMORY, Arrays.asList(Device.Port.DDR5));
        
        devices = Arrays.asList(
            new Device("3", "Gamma", "USA", 300.0, type3, false),
            new Device("1", "Alpha", "China", 100.0, type1, true),
            new Device("2", "Beta", "Taiwan", 200.0, type2, true)
        );
    }
    
    @Test
    void testSortByName() {
        Collections.sort(devices, DeviceComparators.BY_NAME);
        
        assertEquals("Alpha", devices.get(0).getName());
        assertEquals("Beta", devices.get(1).getName());
        assertEquals("Gamma", devices.get(2).getName());
    }
    
    @Test
    void testSortByPriceAsc() {
        Collections.sort(devices, DeviceComparators.BY_PRICE_ASC);
        
        assertEquals(100.0, devices.get(0).getPrice());
        assertEquals(200.0, devices.get(1).getPrice());
        assertEquals(300.0, devices.get(2).getPrice());
    }
    
    @Test
    void testSortByPriceDesc() {
        Collections.sort(devices, DeviceComparators.BY_PRICE_DESC);
        
        assertEquals(300.0, devices.get(0).getPrice());
        assertEquals(200.0, devices.get(1).getPrice());
        assertEquals(100.0, devices.get(2).getPrice());
    }
    
    @Test
    void testSortByOrigin() {
        Collections.sort(devices, DeviceComparators.BY_ORIGIN);
        
        assertEquals("China", devices.get(0).getOrigin());
        assertEquals("Taiwan", devices.get(1).getOrigin());
        assertEquals("USA", devices.get(2).getOrigin());
    }
    
    @Test
    void testSortByPowerConsumption() {
        Collections.sort(devices, DeviceComparators.BY_POWER_CONSUMPTION);
        
        assertEquals(25, devices.get(0).getType().getPowerConsumption());
        assertEquals(50, devices.get(1).getType().getPowerConsumption());
        assertEquals(100, devices.get(2).getType().getPowerConsumption());
    }
    
    @Test
    void testSortByCritical() {
        Collections.sort(devices, DeviceComparators.BY_CRITICAL);
        
        // critical should come first
        assertTrue(devices.get(0).isCritical());
        assertTrue(devices.get(1).isCritical());
        assertFalse(devices.get(2).isCritical());
    }
    
    @Test
    void testSortByCriticalThenPrice() {
        Collections.sort(devices, DeviceComparators.BY_CRITICAL_THEN_PRICE);
        
        // critical first, then sorted by price
        assertTrue(devices.get(0).isCritical());
        assertTrue(devices.get(1).isCritical());
        assertEquals(100.0, devices.get(0).getPrice()); // lower price first
        assertEquals(200.0, devices.get(1).getPrice());
        assertFalse(devices.get(2).isCritical());
    }
}