package com.lab2.parser;

import com.lab2.model.Device;
import org.junit.jupiter.api.BeforeEach;
import org.junit.jupiter.api.Test;
import java.io.ByteArrayInputStream;
import java.io.InputStream;
import java.util.List;
import static org.junit.jupiter.api.Assertions.*;

class SaxParserTest {
    private SaxParser parser;
    private String testXml;
    
    @BeforeEach
    void setUp() {
        parser = new SaxParser();
        testXml = """
            <?xml version="1.0" encoding="UTF-8"?>
            <devices>
                <device id="sax1">
                    <name>SAX Test Device</name>
                    <origin>SAX Country</origin>
                    <price>2000</price>
                    <type>
                        <peripheral>false</peripheral>
                        <powerConsumption>100</powerConsumption>
                        <coolerPresent>true</coolerPresent>
                        <componentGroup>processing</componentGroup>
                        <ports>
                            <port>LGA1700</port>
                            <port>USB</port>
                        </ports>
                    </type>
                    <critical>true</critical>
                </device>
            </devices>
            """;
    }
    
    @Test
    void testSaxParseDevices() throws Exception {
        InputStream xmlStream = new ByteArrayInputStream(testXml.getBytes());
        List<Device> devices = parser.parseDevices(xmlStream);
        
        assertNotNull(devices);
        assertEquals(1, devices.size());
        
        Device device = devices.get(0);
        assertEquals("sax1", device.getId());
        assertEquals("SAX Test Device", device.getName());
        assertEquals("SAX Country", device.getOrigin());
        assertEquals(2000.0, device.getPrice());
        assertTrue(device.isCritical());
    }
    
    @Test
    void testSaxParseComponentType() throws Exception {
        InputStream xmlStream = new ByteArrayInputStream(testXml.getBytes());
        List<Device> devices = parser.parseDevices(xmlStream);
        
        Device.ComponentType type = devices.get(0).getType();
        assertFalse(type.isPeripheral());
        assertEquals(100, type.getPowerConsumption());
        assertTrue(type.isCoolerPresent());
        assertEquals(Device.ComponentGroup.PROCESSING, type.getComponentGroup());
        assertEquals(2, type.getPorts().size());
        assertTrue(type.getPorts().contains(Device.Port.LGA1700));
        assertTrue(type.getPorts().contains(Device.Port.USB));
    }
    
    @Test
    void testSaxParseMultipleDevices() throws Exception {
        String multiXml = """
            <?xml version="1.0"?>
            <devices>
                <device id="dev1">
                    <name>Device 1</name>
                    <origin>Country 1</origin>
                    <price>100</price>
                    <type>
                        <peripheral>true</peripheral>
                        <powerConsumption>10</powerConsumption>
                        <coolerPresent>false</coolerPresent>
                        <componentGroup>input-output</componentGroup>
                        <ports><port>USB</port></ports>
                    </type>
                    <critical>false</critical>
                </device>
                <device id="dev2">
                    <name>Device 2</name>
                    <origin>Country 2</origin>
                    <price>200</price>
                    <type>
                        <peripheral>false</peripheral>
                        <powerConsumption>20</powerConsumption>
                        <coolerPresent>true</coolerPresent>
                        <componentGroup>memory</componentGroup>
                        <ports><port>DDR5</port></ports>
                    </type>
                    <critical>true</critical>
                </device>
            </devices>
            """;
        
        InputStream xmlStream = new ByteArrayInputStream(multiXml.getBytes());
        List<Device> devices = parser.parseDevices(xmlStream);
        
        assertEquals(2, devices.size());
        assertEquals("dev1", devices.get(0).getId());
        assertEquals("dev2", devices.get(1).getId());
    }
}