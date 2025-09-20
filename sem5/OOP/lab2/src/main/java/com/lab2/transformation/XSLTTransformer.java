package com.lab2.transformation;

import javax.xml.transform.Transformer;
import javax.xml.transform.TransformerException;
import javax.xml.transform.TransformerFactory;
import javax.xml.transform.stream.StreamResult;
import javax.xml.transform.stream.StreamSource;
import java.io.InputStream;
import java.io.StringWriter;

public class XSLTTransformer {
    
    public String transformXml(InputStream xmlStream, InputStream xsltStream) throws TransformerException {
        TransformerFactory factory = TransformerFactory.newInstance();
        Transformer transformer = factory.newTransformer(new StreamSource(xsltStream));
        
        StringWriter writer = new StringWriter();
        transformer.transform(new StreamSource(xmlStream), new StreamResult(writer));
        
        return writer.toString();
    }
}