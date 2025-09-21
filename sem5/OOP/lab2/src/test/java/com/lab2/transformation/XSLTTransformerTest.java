package com.lab2.transformation;

import org.junit.jupiter.api.BeforeEach;
import org.junit.jupiter.api.Test;
import java.io.ByteArrayInputStream;
import java.io.InputStream;
import static org.junit.jupiter.api.Assertions.*;

class XSLTTransformerTest {
    private XSLTTransformer transformer;
    private String testXml;
    private String testXsl;
    
    @BeforeEach
    void setUp() {
        transformer = new XSLTTransformer();
        
        testXml = """
            <?xml version="1.0" encoding="UTF-8"?>
            <devices>
                <device id="test1">
                    <n>Test Device</n>
                    <price>1000</price>
                </device>
            </devices>
            """;
        
        testXsl = """
            <?xml version="1.0" encoding="UTF-8"?>
            <xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
                <xsl:output method="html" indent="yes"/>
                <xsl:template match="/">
                    <html>
                        <body>
                            <h1>Device Report</h1>
                            <xsl:for-each select="devices/device">
                                <p><xsl:value-of select="n"/> - $<xsl:value-of select="price"/></p>
                            </xsl:for-each>
                        </body>
                    </html>
                </xsl:template>
            </xsl:stylesheet>
            """;
    }
    
    @Test
    void testXSLTTransformation() throws Exception {
        InputStream xmlStream = new ByteArrayInputStream(testXml.getBytes());
        InputStream xslStream = new ByteArrayInputStream(testXsl.getBytes());
        
        String result = transformer.transformXml(xmlStream, xslStream);
        
        assertNotNull(result);
        assertFalse(result.isEmpty());
        assertTrue(result.contains("<html>"));
        assertTrue(result.contains("Device Report"));
        assertTrue(result.contains("Test Device"));
        assertTrue(result.contains("1000"));
    }
    
    @Test
    void testTransformationWithMultipleDevices() throws Exception {
        String multiDeviceXml = """
            <?xml version="1.0"?>
            <devices>
                <device id="dev1">
                    <n>Device One</n>
                    <price>100</price>
                </device>
                <device id="dev2">
                    <n>Device Two</n>
                    <price>200</price>
                </device>
            </devices>
            """;
        
        InputStream xmlStream = new ByteArrayInputStream(multiDeviceXml.getBytes());
        InputStream xslStream = new ByteArrayInputStream(testXsl.getBytes());
        
        String result = transformer.transformXml(xmlStream, xslStream);
        
        assertTrue(result.contains("Device One"));
        assertTrue(result.contains("Device Two"));
        assertTrue(result.contains("100"));
        assertTrue(result.contains("200"));
    }
    
    @Test
    void testEmptyTransformation() throws Exception {
        String emptyXml = "<?xml version=\"1.0\"?><devices></devices>";
        
        InputStream xmlStream = new ByteArrayInputStream(emptyXml.getBytes());
        InputStream xslStream = new ByteArrayInputStream(testXsl.getBytes());
        
        String result = transformer.transformXml(xmlStream, xslStream);
        
        assertNotNull(result);
        assertTrue(result.contains("<html>"));
        assertTrue(result.contains("Device Report"));
    }
}