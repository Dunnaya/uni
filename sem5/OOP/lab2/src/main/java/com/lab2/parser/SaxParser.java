package com.lab2.parser;

import com.lab2.model.Device;
import com.lab2.model.Device.ComponentGroup;
import com.lab2.model.Device.ComponentType;
import com.lab2.model.Device.Port;
import org.xml.sax.Attributes;
import org.xml.sax.SAXException;
import org.xml.sax.helpers.DefaultHandler;

import javax.xml.parsers.ParserConfigurationException;
import javax.xml.parsers.SAXParser;
import javax.xml.parsers.SAXParserFactory;
import java.io.IOException;
import java.io.InputStream;
import java.util.ArrayList;
import java.util.List;

public class SaxParser {

    // parse devices from XML input stream
    public List<Device> parseDevices(InputStream xmlStream) 
            throws ParserConfigurationException, SAXException, IOException {
        
        SAXParserFactory factory = SAXParserFactory.newInstance();
        SAXParser parser = factory.newSAXParser();
        
        DeviceHandler handler = new DeviceHandler();
        parser.parse(xmlStream, handler);
        
        return handler.getDevices();
    }
    
    // inner class to handle SAX events
    private static class DeviceHandler extends DefaultHandler {
        private List<Device> devices = new ArrayList<>();
        private Device currentDevice;
        private ComponentType currentType;
        private List<Port> currentPorts;
        private StringBuilder textContent = new StringBuilder();
        private String currentElement;
        
        @Override
        public void startElement(String uri, String localName, String qName, 
                               Attributes attributes) throws SAXException {
            currentElement = qName;
            textContent.setLength(0);
            
            switch (qName) {
                case "device":
                    currentDevice = new Device();
                    currentDevice.setId(attributes.getValue("id"));
                    break;
                case "type":
                    currentType = new ComponentType();
                    break;
                case "ports":
                    currentPorts = new ArrayList<>();
                    break;
            }
        }
        
        @Override
        public void characters(char[] ch, int start, int length) throws SAXException {
            textContent.append(ch, start, length);
        }
        
        @Override
        public void endElement(String uri, String localName, String qName) throws SAXException {
            String content = textContent.toString().trim();
            
            switch (qName) {
                case "n":
                    currentDevice.setName(content);
                    break;
                case "origin":
                    currentDevice.setOrigin(content);
                    break;
                case "price":
                    currentDevice.setPrice(Double.parseDouble(content));
                    break;
                case "critical":
                    currentDevice.setCritical(Boolean.parseBoolean(content));
                    break;
                case "peripheral":
                    currentType.setPeripheral(Boolean.parseBoolean(content));
                    break;
                case "powerConsumption":
                    currentType.setPowerConsumption(Integer.parseInt(content));
                    break;
                case "coolerPresent":
                    currentType.setCoolerPresent(Boolean.parseBoolean(content));
                    break;
                case "componentGroup":
                    currentType.setComponentGroup(ComponentGroup.fromValue(content));
                    break;
                case "port":
                    currentPorts.add(Port.fromValue(content));
                    break;
                case "ports":
                    currentType.setPorts(currentPorts);
                    break;
                case "type":
                    currentDevice.setType(currentType);
                    break;
                case "device":
                    devices.add(currentDevice);
                    break;
            }
        }
        
        public List<Device> getDevices() {
            return devices;
        }
    }
}