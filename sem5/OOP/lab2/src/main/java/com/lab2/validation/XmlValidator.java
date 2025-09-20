package com.lab2.validation;

import org.xml.sax.ErrorHandler;
import org.xml.sax.SAXException;
import org.xml.sax.SAXParseException;

import javax.xml.XMLConstants;
import javax.xml.transform.stream.StreamSource;
import javax.xml.validation.Schema;
import javax.xml.validation.SchemaFactory;
import javax.xml.validation.Validator;
import java.io.IOException;
import java.io.InputStream;
import java.util.ArrayList;
import java.util.List;

public class XmlValidator {
    
    public static class ValidationResult {
        private boolean valid;
        private List<String> errors;
        
        public ValidationResult(boolean valid, List<String> errors) {
            this.valid = valid;
            this.errors = errors;
        }
        
        public boolean isValid() { return valid; }
        public List<String> getErrors() { return errors; }
    }
    
    public ValidationResult validateXml(InputStream xmlStream, InputStream xsdStream) {
        List<String> errors = new ArrayList<>();
        
        try {
            SchemaFactory factory = SchemaFactory.newInstance(XMLConstants.W3C_XML_SCHEMA_NS_URI);
            Schema schema = factory.newSchema(new StreamSource(xsdStream));
            
            Validator validator = schema.newValidator();
            validator.setErrorHandler(new ErrorHandler() {
                @Override
                public void warning(SAXParseException exception) throws SAXException {
                    errors.add("Warning: " + exception.getMessage());
                }
                
                @Override
                public void error(SAXParseException exception) throws SAXException {
                    errors.add("Error: " + exception.getMessage());
                }
                
                @Override
                public void fatalError(SAXParseException exception) throws SAXException {
                    errors.add("Fatal Error: " + exception.getMessage());
                }
            });
            
            validator.validate(new StreamSource(xmlStream));
            
            return new ValidationResult(errors.isEmpty(), errors);
            
        } catch (SAXException | IOException e) {
            errors.add("Validation failed: " + e.getMessage());
            return new ValidationResult(false, errors);
        }
    }
}