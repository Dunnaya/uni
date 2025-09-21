package com.lab2.validation;

import org.junit.jupiter.api.BeforeEach;
import org.junit.jupiter.api.Test;
import java.io.ByteArrayInputStream;
import java.io.InputStream;
import static org.junit.jupiter.api.Assertions.*;

class XmlValidatorTest {
    private XmlValidator validator;
    private String validXml;
    private String invalidXml;
    private String xsdSchema;
    
    @BeforeEach
    void setUp() {
        validator = new XmlValidator();
        
        xsdSchema = """
            <?xml version="1.0" encoding="UTF-8"?>
            <xs:schema xmlns:xs="http://www.w3.org/2001/XMLSchema">
                <xs:element name="devices">
                    <xs:complexType>
                        <xs:sequence>
                            <xs:element name="device" type="DeviceType" maxOccurs="unbounded"/>
                        </xs:sequence>
                    </xs:complexType>
                </xs:element>
                
                <xs:complexType name="DeviceType">
                    <xs:sequence>
                        <xs:element name="name" type="xs:string"/>
                        <xs:element name="price" type="xs:decimal"/>
                    </xs:sequence>
                    <xs:attribute name="id" type="xs:ID" use="required"/>
                </xs:complexType>
            </xs:schema>
            """;
        
        validXml = """
            <?xml version="1.0" encoding="UTF-8"?>
            <devices>
                <device id="test1">
                    <name>Test Device</name>
                    <price>100</price>
                </device>
            </devices>
            """;
        
        invalidXml = """
            <?xml version="1.0" encoding="UTF-8"?>
            <devices>
                <device>
                    <name>Test Device</name>
                    <price>invalid_price</price>
                </device>
            </devices>
            """;
    }
    
    @Test
    void testValidXml() {
        InputStream xmlStream = new ByteArrayInputStream(validXml.getBytes());
        InputStream xsdStream = new ByteArrayInputStream(xsdSchema.getBytes());
        
        XmlValidator.ValidationResult result = validator.validateXml(xmlStream, xsdStream);
        
        assertTrue(result.isValid());
        assertTrue(result.getErrors().isEmpty());
    }
    
    @Test
    void testInvalidXml() {
        InputStream xmlStream = new ByteArrayInputStream(invalidXml.getBytes());
        InputStream xsdStream = new ByteArrayInputStream(xsdSchema.getBytes());
        
        XmlValidator.ValidationResult result = validator.validateXml(xmlStream, xsdStream);
        
        assertFalse(result.isValid());
        assertFalse(result.getErrors().isEmpty());
    }
    
    @Test
    void testValidationResultClass() {
        XmlValidator.ValidationResult validResult = new XmlValidator.ValidationResult(true, 
            java.util.Collections.emptyList());
        assertTrue(validResult.isValid());
        assertTrue(validResult.getErrors().isEmpty());
        
        java.util.List<String> errors = java.util.Arrays.asList("Error 1", "Error 2");
        XmlValidator.ValidationResult invalidResult = new XmlValidator.ValidationResult(false, errors);
        assertFalse(invalidResult.isValid());
        assertEquals(2, invalidResult.getErrors().size());
        assertEquals("Error 1", invalidResult.getErrors().get(0));
    }
}