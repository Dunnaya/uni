package com.lab2.parser;

import com.lab2.model.Device;
import com.lab2.model.Device.ComponentGroup;
import com.lab2.model.Device.ComponentType;
import com.lab2.model.Device.Port;
import org.w3c.dom.Document;
import org.w3c.dom.Element;
import org.w3c.dom.Node;
import org.w3c.dom.NodeList;
import org.xml.sax.SAXException;

import javax.xml.parsers.DocumentBuilder;
import javax.xml.parsers.DocumentBuilderFactory;
import javax.xml.parsers.ParserConfigurationException;
import java.io.IOException;
import java.io.InputStream;
import java.util.ArrayList;
import java.util.List;

public class DOMParser {
    public List<Device> parseDevices(InputStream xmlStream) 
            throws ParserConfigurationException, SAXException, IOException {
        
        DocumentBuilderFactory factory = DocumentBuilderFactory.newInstance();
        DocumentBuilder builder = factory.newDocumentBuilder();
        Document document = builder.parse(xmlStream);
        
        document.getDocumentElement().normalize();
        
        List<Device> devices = new ArrayList<>();
        NodeList deviceNodes = document.getElementsByTagName("device");
        
        for (int i = 0; i < deviceNodes.getLength(); i++) {
            Node deviceNode = deviceNodes.item(i);
            if (deviceNode.getNodeType() == Node.ELEMENT_NODE) {
                devices.add(parseDevice((Element) deviceNode));
            }
        }
        
        return devices;
    }
    
    // parse a single device
    private Device parseDevice(Element deviceElement) {
        Device device = new Device();

        // ID attribute
        device.setId(deviceElement.getAttribute("id"));
        
        // main fields
        device.setName(getElementText(deviceElement, "name"));
        device.setOrigin(getElementText(deviceElement, "origin"));
        device.setPrice(Double.parseDouble(getElementText(deviceElement, "price")));
        device.setCritical(Boolean.parseBoolean(getElementText(deviceElement, "critical")));
        
        // component type
        Element typeElement = (Element) deviceElement.getElementsByTagName("type").item(0);
        device.setType(parseComponentType(typeElement));
        
        return device;
    }
    
    // parse component type
    private ComponentType parseComponentType(Element typeElement) {
        ComponentType type = new ComponentType();
        
        type.setPeripheral(Boolean.parseBoolean(getElementText(typeElement, "peripheral")));
        type.setPowerConsumption(Integer.parseInt(getElementText(typeElement, "powerConsumption")));
        type.setCoolerPresent(Boolean.parseBoolean(getElementText(typeElement, "coolerPresent")));
        type.setComponentGroup(ComponentGroup.fromValue(getElementText(typeElement, "componentGroup")));
        
        // parsing ports
        Element portsElement = (Element) typeElement.getElementsByTagName("ports").item(0);
        type.setPorts(parsePorts(portsElement));
        
        return type;
    }

    // parse ports
    private List<Port> parsePorts(Element portsElement) {
        List<Port> ports = new ArrayList<>();
        NodeList portNodes = portsElement.getElementsByTagName("port");
        
        for (int i = 0; i < portNodes.getLength(); i++) {
            Node portNode = portNodes.item(i);
            if (portNode.getNodeType() == Node.ELEMENT_NODE) {
                ports.add(Port.fromValue(portNode.getTextContent().trim()));
            }
        }
        
        return ports;
    }
    
    // get text content of an element
    private String getElementText(Element parent, String tagName) {
        NodeList nodeList = parent.getElementsByTagName(tagName);
        if (nodeList.getLength() > 0) {
            return nodeList.item(0).getTextContent().trim();
        }
        return "";
    }
}