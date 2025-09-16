package com.lab1.gift;

import com.lab1.sweets.Sweet;
import java.util.ArrayList;
import java.util.Collections;
import java.util.Comparator;
import java.util.List;

public class Gift {
    private ArrayList<Sweet> sweets;

    public Gift() {
        sweets = new ArrayList<>();
    }

    public void addSweet(Sweet sweet) {
        if (sweet == null) {
            throw new IllegalArgumentException("Sweet cannot be null");
        }
        sweets.add(sweet);
    }

    public void showGift() {
        if (sweets.isEmpty()) {
            System.out.println("The gift is empty");
            return;
        }
        
        for (int i = 0; i < sweets.size(); i++) {
            System.out.printf("%d. %s%n", i + 1, sweets.get(i));
        }
    }

    public double getTotalWeight() {
        double totalWeight = 0;
        for (Sweet sweet : sweets) {
            totalWeight += sweet.getWeight();
        }
        return totalWeight;
    }

    public void sortByCalories() {
        Collections.sort(sweets, Comparator.comparingDouble(Sweet::getCalories));
    }

    public List<Sweet> findBySugarRange(double minSugar, double maxSugar) {
        if (minSugar < 0 || maxSugar < 0) {
            throw new IllegalArgumentException("Sugar values cannot be negative");
        }
        if (minSugar > maxSugar) {
            throw new IllegalArgumentException("Min sugar cannot be greater than max sugar");
        }
        
        List<Sweet> result = new ArrayList<>();
        for (Sweet sweet : sweets) {
            if (sweet.getSugar() >= minSugar && sweet.getSugar() <= maxSugar) {
                result.add(sweet);
            }
        }
        return result;
    }

    public List<Sweet> getSweets() {
        return new ArrayList<>(sweets);
    }

    public int size() {
        return sweets.size();
    }

    public boolean isEmpty() {
        return sweets.isEmpty();
    }

    public void clear() {
        sweets.clear();
    }
}