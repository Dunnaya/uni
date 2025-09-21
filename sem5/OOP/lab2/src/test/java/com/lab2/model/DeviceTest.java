package com.lab2.model;

import org.junit.jupiter.api.BeforeEach;
import org.junit.jupiter.api.Test;
import java.util.Arrays;
import static org.junit.jupiter.api.Assertions.*;

class DeviceTest {
    private Device device;
    private Device.ComponentType componentType;
    
    @BeforeEach
    void setUp() {
        componentType = new Device.ComponentType(
            false, 
            220, 
            true, 
            Device.ComponentGroup.MULTIMEDIA,
            Arrays.asList(Device.Port.USB, Device.Port.HDMI)
        );
        
        device = new Device("device1", "NVIDIA GeForce RTX 4070", 
                           "Taiwan", 25000.0, componentType, true);
    }
    
    @Test
    void testDeviceCreation() {
        assertNotNull(device);
        assertEquals("device1", device.getId());
        assertEquals("NVIDIA GeForce RTX 4070", device.getName());
        assertEquals("Taiwan", device.getOrigin());
        assertEquals(25000.0, device.getPrice());
        assertTrue(device.isCritical());
    }
    
    @Test
    void testComponentType() {
        assertNotNull(device.getType());
        assertFalse(device.getType().isPeripheral());
        assertEquals(220, device.getType().getPowerConsumption());
        assertTrue(device.getType().isCoolerPresent());
        assertEquals(Device.ComponentGroup.MULTIMEDIA, device.getType().getComponentGroup());
        assertEquals(2, device.getType().getPorts().size());
    }
    
    @Test
    void testDeviceEquals() {
        Device sameDevice = new Device("device1", "Another Name", 
                                     "USA", 1000.0, componentType, false);
        assertEquals(device, sameDevice); // same ID
        
        Device differentDevice = new Device("device2", "Same Name", 
                                          "Taiwan", 25000.0, componentType, true);
        assertNotEquals(device, differentDevice); // different ID
    }
    
    @Test
    void testComponentGroupFromValue() {
        assertEquals(Device.ComponentGroup.MULTIMEDIA, 
                    Device.ComponentGroup.fromValue("multimedia"));
        assertEquals(Device.ComponentGroup.INPUT_OUTPUT, 
                    Device.ComponentGroup.fromValue("input-output"));
        
        assertThrows(IllegalArgumentException.class, 
                    () -> Device.ComponentGroup.fromValue("invalid"));
    }
    
    @Test
    void testPortFromValue() {
        assertEquals(Device.Port.USB, Device.Port.fromValue("usb"));
        assertEquals(Device.Port.HDMI, Device.Port.fromValue("HDMI"));
        
        assertThrows(IllegalArgumentException.class, 
                    () -> Device.Port.fromValue("invalid"));
    }
}