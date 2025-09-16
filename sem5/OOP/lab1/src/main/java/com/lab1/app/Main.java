package com.lab1.app;

import com.lab1.gift.Gift;
import com.lab1.sweets.*;
import java.util.Scanner;
import java.util.List;

public class Main {
    private static Gift gift = new Gift();
    private static Scanner scanner = new Scanner(System.in);

    public static void main(String[] args) {
        initializeGift();
        showMenu();
    }

    private static void initializeGift() {
        gift.addSweet(new Chocolate("Milka", 100.0, 534.0, 59.0, "30%", false, Chocolate.ChocolateType.MILK));
        gift.addSweet(new Chocolate("Roshen Dark", 85.0, 551.0, 18.2, "80%", true, Chocolate.ChocolateType.DARK));
        gift.addSweet(new Candy("Chupa Chups", 12.0, 370.0, 91.0, "Strawberry", true));
        gift.addSweet(new Candy("Cow", 15.0, 379.0, 67.8, "Caramel", false));
        gift.addSweet(new Jelly("Haribo Bears", 3.0, 392.0, 59.0, "Bear", false));
        gift.addSweet(new Jelly("Sour Worms", 5.0, 338.0, 75.0, "Worm", true));
        System.out.println("New Year's gift initialized!");
    }

    private static void showMenu() {
        while (true) {
            System.out.println("\n=== NEW YEAR'S GIFT ===");
            System.out.println("1. Show all sweets in the gift");
            System.out.println("2. Show total weight of the gift");
            System.out.println("3. Sort sweets by calories");
            System.out.println("4. Find sweets by sugar range");
            System.out.println("5. Add a new sweet");
            System.out.println("0. Exit");
            System.out.print("Choose an option: ");

            int choice = readIntInRange(0, 5);

            switch (choice) {
                case 1 -> showAllSweets();
                case 2 -> showTotalWeight();
                case 3 -> sortByCalories();
                case 4 -> findBySugarRange();
                case 5 -> addNewSweet();
                case 0 -> {
                    System.out.println("Goodbye and Happy New Year!");
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
                    System.out.println("Invalid choice. Enter a number between " + min + " and " + max + ".");
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
                    System.out.println("Value must be positive. Try again.");
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
            System.out.println("Invalid input. Enter true or false.");
        }
    }

    // yes, sugar cannot exceed weight, but it's g/100g, so it's possible... fuck my stupid baka life...
    
    // private static double readSugar(double weight) { 
    //     while (true) {
    //         double sugar = readPositiveDouble("Sugar (g/100g): ");
    //         if (sugar > weight) {
    //             System.out.println("Sugar cannot exceed total weight of the product (" + weight + " g). Try again.");
    //             continue;
    //         }
    //         return sugar;
    //     }
    // }

    // --- basic actions ---
    private static void showAllSweets() {
        System.out.println("\nSweets in the gift:");
        gift.showGift();
    }

    private static void showTotalWeight() {
        System.out.printf("\nTotal weight of the gift: %.1f g%n", gift.getTotalWeight());
    }

    private static void sortByCalories() {
        gift.sortByCalories();
        System.out.println("\nSweets sorted by calories:");
        gift.showGift();
    }

    private static void findBySugarRange() {
        double minSugar, maxSugar;
        while (true) {
            minSugar = readPositiveDouble("Enter minimum sugar content (g/100g): ");
            maxSugar = readPositiveDouble("Enter maximum sugar content (g/100g): ");
            if (minSugar > maxSugar) {
                System.out.println("Minimum sugar cannot be greater than maximum sugar. Try again.");
                continue;
            }
            break;
        }

        List<Sweet> found = gift.findBySugarRange(minSugar, maxSugar);
        System.out.printf("\nSweets with sugar content from %.1f to %.1f g/100g:%n", minSugar, maxSugar);
        if (found.isEmpty()) System.out.println("No sweets found in the specified range.");
        else found.forEach(System.out::println);
    }

    private static void addNewSweet() {
        System.out.println("\nChoose sweet type:");
        System.out.println("1. Chocolate");
        System.out.println("2. Candy");
        System.out.println("3. Jelly");
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

        System.out.println("The sweet has been added!");
    }

    // --- adding specific types ---
    private static void addChocolate(String name, double weight, double calories, double sugar) {
        System.out.print("Cocoa content: ");
        String cocoa = scanner.nextLine();
        boolean hasNuts = readBoolean("Contains nuts? (t/f): ");
        System.out.println("Chocolate type (1-MILK, 2-DARK, 3-WHITE): ");
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
