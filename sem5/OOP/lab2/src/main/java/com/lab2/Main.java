package com.lab2;

import com.lab2.model.Device;
import com.lab2.parser.DOMParser;
import com.lab2.parser.SaxParser;
import com.lab2.parser.StAXParser;
import com.lab2.transformation.XSLTTransformer;
import com.lab2.util.DeviceComparators;
import com.lab2.validation.XmlValidator;

import java.io.InputStream;
import java.util.Collections;
import java.util.List;
import java.util.Scanner;

public class Main {
    public static void main(String[] args) {
        Scanner scanner = new Scanner(System.in);
        
        while (true) {
            showMenu();
            int choice = getChoice(scanner);
            
            try {
                switch (choice) {
                    case 1:
                        testDOMParser();
                        break;
                    case 2:
                        testSAXParser();
                        break;
                    case 3:
                        testStAXParser();
                        break;
                    case 4:
                        testValidation();
                        break;
                    case 5:
                        testSorting();
                        break;
                    case 6:
                        testXSLTransformation();
                        break;
                    case 0:
                        System.out.println("Exit...");
                        scanner.close();
                        return;
                    default:
                        System.out.println("Invalid choice. Please try again.");
                }
            } catch (Exception e) {
                System.err.println("Error: " + e.getMessage());
                e.printStackTrace();
            }
        }
    }
    
    private static void showMenu() {
        System.out.println("\n" + "-".repeat(50));
        System.out.println("1. DOM Parser");
        System.out.println("2. SAX Parser");
        System.out.println("3. StAX Parser");
        System.out.println("4. XML Validation");
        System.out.println("5. Sorting Demonstration");
        System.out.println("6. XSLT Transformation");
        System.out.println("0. Exit");
        System.out.println("-".repeat(50));
        System.out.print("Your choice: ");
    }
    
    private static int getChoice(Scanner scanner) {
        try {
            return Integer.parseInt(scanner.nextLine());
        } catch (NumberFormatException e) {
            return -1;
        }
    }
    
    private static void testDOMParser() throws Exception {
        System.out.println("\n Testing DOM Parser\n");
        DOMParser parser = new DOMParser();
        
        try (InputStream xmlStream = Main.class.getResourceAsStream("/devices.xml")) {
            List<Device> devices = parser.parseDevices(xmlStream);
            printDevices(devices, "DOM Parser");
        }
    }
    
    private static void testSAXParser() throws Exception {
        System.out.println("\n Testing SAX Parser\n");
        SaxParser parser = new SaxParser();
        
        try (InputStream xmlStream = Main.class.getResourceAsStream("/devices.xml")) {
            List<Device> devices = parser.parseDevices(xmlStream);
            printDevices(devices, "SAX Parser");
        }
    }
    
    private static void testStAXParser() throws Exception {
        System.out.println("\n Testing StAX Parser\n");
        StAXParser parser = new StAXParser();
        
        try (InputStream xmlStream = Main.class.getResourceAsStream("/devices.xml")) {
            List<Device> devices = parser.parseDevices(xmlStream);
            printDevices(devices, "StAX Parser");
        }
    }
    
    private static void testValidation() throws Exception {
        System.out.println("\n XML Validation with XSD\n");
        XmlValidator validator = new XmlValidator();
        
        try (InputStream xmlStream = Main.class.getResourceAsStream("/devices.xml");
             InputStream xsdStream = Main.class.getResourceAsStream("/devices.xsd")) {
            
            XmlValidator.ValidationResult result = validator.validateXml(xmlStream, xsdStream);
            
            if (result.isValid()) {
                System.out.println("Yay! XML document is valid!");
            } else {
                System.out.println("Oh no! XML document has errors:");
                result.getErrors().forEach(error -> System.out.println("  - " + error));
            }
        }
    }
    
    private static void testSorting() throws Exception {
        System.out.println("\n Sorting Demonstration\n");
        DOMParser parser = new DOMParser();
        
        try (InputStream xmlStream = Main.class.getResourceAsStream("/devices.xml")) {
            List<Device> devices = parser.parseDevices(xmlStream);

            System.out.println("Sorting by name:");
            Collections.sort(devices, DeviceComparators.BY_NAME);
            devices.forEach(device -> 
                System.out.println("  " + device.getName() + " - " + device.getPrice()));

            System.out.println("\nSorting by price (ascending):");
            Collections.sort(devices, DeviceComparators.BY_PRICE_ASC);
            devices.forEach(device -> 
                System.out.println("  " + device.getName() + " - $" + device.getPrice()));
            
            System.out.println("\nSorting by criticality and price:");
            Collections.sort(devices, DeviceComparators.BY_CRITICAL_THEN_PRICE);
            devices.forEach(device -> 
                System.out.println("  " + device.getName() + 
                    " - Critical: " + device.isCritical() + 
                    " - $" + device.getPrice()));
        }
    }
    
    private static void testXSLTransformation() throws Exception {
        System.out.println("\n XSLT Transformation\n");
        XSLTTransformer transformer = new XSLTTransformer();
        
        try (InputStream xmlStream = Main.class.getResourceAsStream("/devices.xml");
             InputStream xslStream = Main.class.getResourceAsStream("/devices.xsl")) {
            
            String htmlResult = transformer.transformXml(xmlStream, xslStream);
            
            // save to file
            String fileName = "devices_report.html";
            try (java.io.FileWriter writer = new java.io.FileWriter(fileName)) {
                writer.write(htmlResult);
            }

            System.out.println(" HTML report: " + fileName);
            System.out.println("File size: " + htmlResult.length() + " characters");
            System.out.println("\nOpen the file in your browser for the full report.");
        }
    }
    
    private static void printDevices(List<Device> devices, String parserType) {
        System.out.println("Found " + devices.size() + " devices using " + parserType + ":");
        System.out.println("-".repeat(80));
        
        for (Device device : devices) {
            System.out.printf("ID: %-10s | %-25s | %8s | ₴%-8.0f | %s%n",
                device.getId(),
                device.getName(),
                device.getOrigin(),
                device.getPrice(),
                device.isCritical() ? "Critical" : "Optional"
            );
        }
    }
}