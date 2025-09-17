package com.lab1.config;

import com.lab1.gift.Gift;
import com.lab1.sweets.*;
import org.w3c.dom.*;
import javax.xml.parsers.DocumentBuilder;
import javax.xml.parsers.DocumentBuilderFactory;
import java.io.InputStream;

public class XMLConfigLoader {
    public static Gift loadGiftFromXML() {
        Gift gift = new Gift();

        try {
            DocumentBuilderFactory factory = DocumentBuilderFactory.newInstance();
            factory.setValidating(false); // simple validation
            DocumentBuilder builder = factory.newDocumentBuilder();

            // loading XML as resource
            InputStream inputStream = XMLConfigLoader.class
                    .getClassLoader()
                    .getResourceAsStream("gift.xml");

            if (inputStream == null) {
                throw new IllegalArgumentException("gift.xml not found in resources!");
            }

            Document doc = builder.parse(inputStream);
            doc.getDocumentElement().normalize();

            // loading sweets
            NodeList chocolateList = doc.getElementsByTagName("chocolate");
            for (int i = 0; i < chocolateList.getLength(); i++) {
                Element chocolate = (Element) chocolateList.item(i);
                gift.addSweet(createChocolateFromElement(chocolate));
            }

            NodeList candyList = doc.getElementsByTagName("candy");
            for (int i = 0; i < candyList.getLength(); i++) {
                Element candy = (Element) candyList.item(i);
                gift.addSweet(createCandyFromElement(candy));
            }

            NodeList jellyList = doc.getElementsByTagName("jelly");
            for (int i = 0; i < jellyList.getLength(); i++) {
                Element jelly = (Element) jellyList.item(i);
                gift.addSweet(createJellyFromElement(jelly));
            }

        } catch (Exception e) {
            System.err.println("Error loading gift from XML: " + e.getMessage());
            // default gift if XML loading fails
            initializeDefaultGift(gift);
        }

        return gift;
    }

    private static Chocolate createChocolateFromElement(Element element) {
        String name = getElementTextContent(element, "name");
        double weight = Double.parseDouble(getElementTextContent(element, "weight"));
        double calories = Double.parseDouble(getElementTextContent(element, "calories"));
        double sugar = Double.parseDouble(getElementTextContent(element, "sugar"));
        String cocoa = getElementTextContent(element, "cocoa");
        boolean hasNuts = Boolean.parseBoolean(getElementTextContent(element, "hasNuts"));
        Chocolate.ChocolateType type = Chocolate.ChocolateType.valueOf(getElementTextContent(element, "type"));

        return new Chocolate(name, weight, calories, sugar, cocoa, hasNuts, type);
    }

    private static Candy createCandyFromElement(Element element) {
        String name = getElementTextContent(element, "name");
        double weight = Double.parseDouble(getElementTextContent(element, "weight"));
        double calories = Double.parseDouble(getElementTextContent(element, "calories"));
        double sugar = Double.parseDouble(getElementTextContent(element, "sugar"));
        String flavor = getElementTextContent(element, "flavor");
        boolean isHard = Boolean.parseBoolean(getElementTextContent(element, "isHard"));

        return new Candy(name, weight, calories, sugar, flavor, isHard);
    }

    private static Jelly createJellyFromElement(Element element) {
        String name = getElementTextContent(element, "name");
        double weight = Double.parseDouble(getElementTextContent(element, "weight"));
        double calories = Double.parseDouble(getElementTextContent(element, "calories"));
        double sugar = Double.parseDouble(getElementTextContent(element, "sugar"));
        String shape = getElementTextContent(element, "shape");
        boolean isSour = Boolean.parseBoolean(getElementTextContent(element, "isSour"));

        return new Jelly(name, weight, calories, sugar, shape, isSour);
    }

    private static String getElementTextContent(Element parent, String tagName) {
        NodeList nodeList = parent.getElementsByTagName(tagName);
        if (nodeList.getLength() > 0) {
            return nodeList.item(0).getTextContent().trim();
        }
        return "";
    }

    private static void initializeDefaultGift(Gift gift) {
        gift.addSweet(new Jelly("Haribo Bears", 3.0, 392.0, 59.0, "Bear", false));
        gift.addSweet(new Jelly("Sour Worms", 5.0, 338.0, 75.0, "Worm", true));
        gift.addSweet(new Candy("Chupa Chups", 12.0, 370.0, 91.0, "Strawberry", true));
        gift.addSweet(new Candy("Cow", 15.0, 379.0, 67.8, "Caramel", false));
        gift.addSweet(new Chocolate("Milka", 100.0, 534.0, 59.0, "30%", false, Chocolate.ChocolateType.MILK));
        gift.addSweet(new Chocolate("Roshen Dark", 85.0, 551.0, 18.2, "80%", true, Chocolate.ChocolateType.DARK));
    }
}
