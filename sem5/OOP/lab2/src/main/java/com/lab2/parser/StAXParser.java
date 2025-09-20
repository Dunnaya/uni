package com.lab2.parser;

import com.lab2.model.Device;
import com.lab2.model.Device.ComponentGroup;
import com.lab2.model.Device.ComponentType;
import com.lab2.model.Device.Port;

import javax.xml.stream.XMLEventReader;
import javax.xml.stream.XMLInputFactory;
import javax.xml.stream.XMLStreamException;
import javax.xml.stream.events.Attribute;
import javax.xml.stream.events.EndElement;
import javax.xml.stream.events.StartElement;
import javax.xml.stream.events.XMLEvent;
import java.io.InputStream;
import java.util.ArrayList;
import java.util.Iterator;
import java.util.List;

public class StAXParser {

    public List<Device> parseDevices(InputStream xmlStream) throws XMLStreamException {
        List<Device> devices = new ArrayList<>();
        Device currentDevice = null;
        ComponentType currentType = null;
        List<Port> currentPorts = null;
        
        XMLInputFactory factory = XMLInputFactory.newInstance();
        XMLEventReader eventReader = factory.createXMLEventReader(xmlStream);
        
        String currentElement = "";
        
        while (eventReader.hasNext()) {
            XMLEvent event = eventReader.nextEvent();
            
            if (event.isStartElement()) {
                StartElement startElement = event.asStartElement();
                currentElement = startElement.getName().getLocalPart();
                
                switch (currentElement) {
                    case "device":
                        currentDevice = new Device();
                        Iterator<Attribute> attributes = startElement.getAttributes();
                        while (attributes.hasNext()) {
                            Attribute attr = attributes.next();
                            if (attr.getName().toString().equals("id")) {
                                currentDevice.setId(attr.getValue());
                            }
                        }
                        break;
                    case "type":
                        currentType = new ComponentType();
                        break;
                    case "ports":
                        currentPorts = new ArrayList<>();
                        break;
                }
            } else if (event.isCharacters() && currentDevice != null) {
                String content = event.asCharacters().getData().trim();
                if (!content.isEmpty()) {
                    switch (currentElement) {
                        case "name":
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
                    }
                }
            } else if (event.isEndElement()) {
                EndElement endElement = event.asEndElement();
                String elementName = endElement.getName().getLocalPart();
                
                switch (elementName) {
                    case "ports":
                        currentType.setPorts(currentPorts);
                        break;
                    case "type":
                        currentDevice.setType(currentType);
                        break;
                    case "device":
                        devices.add(currentDevice);
                        currentDevice = null;
                        break;
                }
            }
        }
        
        eventReader.close();
        return devices;
    }
}