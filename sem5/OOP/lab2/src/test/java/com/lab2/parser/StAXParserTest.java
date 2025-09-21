package com.lab2.parser;

import com.lab2.model.Device;
import org.junit.jupiter.api.BeforeEach;
import org.junit.jupiter.api.Test;
import java.io.ByteArrayInputStream;
import java.io.InputStream;
import java.util.List;
import static org.junit.jupiter.api.Assertions.*;

class StAXParserTest {
    private StAXParser parser;
    private String testXml;
    
    @BeforeEach
    void setUp() {
        parser = new StAXParser();
        testXml = """
            <?xml version="1.0" encoding="UTF-8"?>
            <devices>
                <device id="stax1">
                    <name>StAX Device</name>
                    <origin>StAX Country</origin>
                    <price>1500</price>
                    <type>
                        <peripheral>true</peripheral>
                        <powerConsumption>75</powerConsumption>
                        <coolerPresent>false</coolerPresent>
                        <componentGroup>multimedia</componentGroup>
                        <ports>
                            <port>HDMI</port>
                        </ports>
                    </type>
                    <critical>false</critical>
                </device>
            </devices>
            """;
    }
    
    @Test
    void testStAXParseDevices() throws Exception {
        InputStream xmlStream = new ByteArrayInputStream(testXml.getBytes());
        List<Device> devices = parser.parseDevices(xmlStream);
        
        assertNotNull(devices);
        assertEquals(1, devices.size());
        
        Device device = devices.get(0);
        assertEquals("stax1", device.getId());
        assertEquals("StAX Device", device.getName());
        assertEquals("StAX Country", device.getOrigin());
        assertEquals(1500.0, device.getPrice());
        assertFalse(device.isCritical());
    }
    
    @Test
    void testStAXParseComponentType() throws Exception {
        InputStream xmlStream = new ByteArrayInputStream(testXml.getBytes());
        List<Device> devices = parser.parseDevices(xmlStream);
        
        Device.ComponentType type = devices.get(0).getType();
        assertTrue(type.isPeripheral());
        assertEquals(75, type.getPowerConsumption());
        assertFalse(type.isCoolerPresent());
        assertEquals(Device.ComponentGroup.MULTIMEDIA, type.getComponentGroup());
        assertEquals(1, type.getPorts().size());
        assertEquals(Device.Port.HDMI, type.getPorts().get(0));
    }
}