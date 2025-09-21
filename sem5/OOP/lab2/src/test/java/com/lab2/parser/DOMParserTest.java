package com.lab2.parser;

import com.lab2.model.Device;
import org.junit.jupiter.api.BeforeEach;
import org.junit.jupiter.api.Test;
import java.io.ByteArrayInputStream;
import java.io.InputStream;
import java.util.List;
import static org.junit.jupiter.api.Assertions.*;

class DOMParserTest {
    private DOMParser parser;
    private String testXml;
    
    @BeforeEach
    void setUp() {
        parser = new DOMParser();
        testXml = """
            <?xml version="1.0" encoding="UTF-8"?>
            <devices>
                <device id="test1">
                    <name>Test Device</name>
                    <origin>Test Country</origin>
                    <price>1000</price>
                    <type>
                        <peripheral>true</peripheral>
                        <powerConsumption>50</powerConsumption>
                        <coolerPresent>false</coolerPresent>
                        <componentGroup>input-output</componentGroup>
                        <ports>
                            <port>USB</port>
                        </ports>
                    </type>
                    <critical>false</critical>
                </device>
            </devices>
            """;
    }
    
    @Test
    void testParseDevices() throws Exception {
        InputStream xmlStream = new ByteArrayInputStream(testXml.getBytes());
        List<Device> devices = parser.parseDevices(xmlStream);
        
        assertNotNull(devices);
        assertEquals(1, devices.size());
        
        Device device = devices.get(0);
        assertEquals("test1", device.getId());
        assertEquals("Test Device", device.getName());
        assertEquals("Test Country", device.getOrigin());
        assertEquals(1000.0, device.getPrice());
        assertFalse(device.isCritical());
    }
    
    @Test
    void testParseComponentType() throws Exception {
        InputStream xmlStream = new ByteArrayInputStream(testXml.getBytes());
        List<Device> devices = parser.parseDevices(xmlStream);
        
        Device.ComponentType type = devices.get(0).getType();
        assertTrue(type.isPeripheral());
        assertEquals(50, type.getPowerConsumption());
        assertFalse(type.isCoolerPresent());
        assertEquals(Device.ComponentGroup.INPUT_OUTPUT, type.getComponentGroup());
        assertEquals(1, type.getPorts().size());
        assertEquals(Device.Port.USB, type.getPorts().get(0));
    }
    
    @Test
    void testParseEmptyXml() throws Exception {
        String emptyXml = "<?xml version=\"1.0\"?><devices></devices>";
        InputStream xmlStream = new ByteArrayInputStream(emptyXml.getBytes());
        
        List<Device> devices = parser.parseDevices(xmlStream);
        assertNotNull(devices);
        assertTrue(devices.isEmpty());
    }
}