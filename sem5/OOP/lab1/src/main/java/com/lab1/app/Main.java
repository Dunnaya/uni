package com.lab1.app;

import com.lab1.config.XMLConfigLoader;
import com.lab1.gift.Gift;
import com.lab1.sweets.*;
import java.util.Scanner;
import java.util.List;

public class Main {
    private static Gift gift;
    private static Scanner scanner = new Scanner(System.in);

    public static void main(String[] args) {
        initializeGift();
        showMenu();
    }

    private static void initializeGift() {
        // fallback to hardcoded values if XML not found
        gift = XMLConfigLoader.loadGiftFromXML();
        System.out.println("\nNew Year's gift initialized!");
    }

    private static void showMenu() {
        while (true) {
            System.out.println("\n=== NEW YEAR'S GIFT ===");
            System.out.println("1. Show all sweets");
            System.out.println("2. Show total weight");
            System.out.println("3. Sort by calories");
            System.out.println("4. Find by sugar range");
            System.out.println("5. Add sweet");
            System.out.println("0. Exit");
            System.out.print("Choice: ");

            int choice = readIntInRange(0, 5);

            switch (choice) {
                case 1 -> showAllSweets();
                case 2 -> showTotalWeight();
                case 3 -> sortByCalories();
                case 4 -> findBySugarRange();
                case 5 -> addNewSweet();
                case 0 -> {
                    System.out.println("Goodbye!");
                    return;
                }
            }
        }
    }

    // --- input methods ---
    private static int readIntInRange(int min, int max) {
        while (true) {
            String input = scanner.nextLine();
            try {
                int value = Integer.parseInt(input);
                if (value < min || value > max) {
                    System.out.println("Invalid choice. Enter " + min + "-" + max + ".");
                    continue;
                }
                return value;
            } catch (NumberFormatException e) {
                System.out.println("Invalid input. Enter a valid number.");
            }
        }
    }

    private static double readPositiveDouble(String prompt) {
        while (true) {
            System.out.print(prompt);
            String input = scanner.nextLine();
            try {
                double value = Double.parseDouble(input);
                if (value <= 0) {
                    System.out.println("Value must be positive.");
                    continue;
                }
                return value;
            } catch (NumberFormatException e) {
                System.out.println("Invalid input. Enter a valid number.");
            }
        }
    }

    private static boolean readBoolean(String prompt) {
        while (true) {
            System.out.print(prompt);
            String input = scanner.nextLine().trim().toLowerCase();
            if (input.equals("t")) return true;
            if (input.equals("f")) return false;
            System.out.println("Invalid input. Enter t or f.");
        }
    }

    // --- basic actions ---
    private static void showAllSweets() {
        System.out.println("\nSweets in the gift:");
        gift.showGift();
    }

    private static void showTotalWeight() {
        System.out.printf("\nTotal weight: %.1f g%n", gift.getTotalWeight());
    }

    private static void sortByCalories() {
        gift.sortByCalories();
        System.out.println("\nSorted by calories:");
        gift.showGift();
    }

    private static void findBySugarRange() {
        double minSugar, maxSugar;
        while (true) {
            minSugar = readPositiveDouble("Min sugar (g/100g): ");
            maxSugar = readPositiveDouble("Max sugar (g/100g): ");
            if (minSugar > maxSugar) {
                System.out.println("Min cannot be greater than max.");
                continue;
            }
            break;
        }

        List<Sweet> found = gift.findBySugarRange(minSugar, maxSugar);
        System.out.printf("\nSweets with sugar %.1f-%.1f g/100g:%n", minSugar, maxSugar);
        if (found.isEmpty()) System.out.println("No sweets found.");
        else found.forEach(System.out::println);
    }

    private static void addNewSweet() {
        System.out.println("\nChoose type:");
        System.out.println("1. Chocolate  2. Candy  3. Jelly");
        int type = readIntInRange(1, 3);

        System.out.print("Name: ");
        String name = scanner.nextLine();
        double weight = readPositiveDouble("Weight (g): ");
        double calories = readPositiveDouble("Calories (kcal/100g): ");
        double sugar = readPositiveDouble("Sugar (g/100g): ");

        switch (type) {
            case 1 -> addChocolate(name, weight, calories, sugar);
            case 2 -> addCandy(name, weight, calories, sugar);
            case 3 -> addJelly(name, weight, calories, sugar);
        }

        System.out.println("Sweet added!");
    }

    // --- adding specific types ---
    private static void addChocolate(String name, double weight, double calories, double sugar) {
        System.out.print("Cocoa content: ");
        String cocoa = scanner.nextLine();
        boolean hasNuts = readBoolean("Has nuts? (t/f): ");
        System.out.println("Type (1-MILK, 2-DARK, 3-WHITE): ");
        int typeChoice = readIntInRange(1, 3);
        Chocolate.ChocolateType type = switch (typeChoice) {
            case 1 -> Chocolate.ChocolateType.MILK;
            case 2 -> Chocolate.ChocolateType.DARK;
            case 3 -> Chocolate.ChocolateType.WHITE;
            default -> Chocolate.ChocolateType.MILK;
        };
        gift.addSweet(new Chocolate(name, weight, calories, sugar, cocoa, hasNuts, type));
    }

    private static void addCandy(String name, double weight, double calories, double sugar) {
        System.out.print("Flavor: ");
        String flavor = scanner.nextLine();
        boolean isHard = readBoolean("Is hard? (t/f): ");
        gift.addSweet(new Candy(name, weight, calories, sugar, flavor, isHard));
    }

    private static void addJelly(String name, double weight, double calories, double sugar) {
        System.out.print("Shape: ");
        String shape = scanner.nextLine();
        boolean isSour = readBoolean("Is sour? (t/f): ");
        gift.addSweet(new Jelly(name, weight, calories, sugar, shape, isSour));
    }
}