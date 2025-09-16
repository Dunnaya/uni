package com.lab1.gift;

import com.lab1.sweets.Sweet;
import java.util.ArrayList;
import java.util.Collections;
import java.util.Comparator;

public class Gift {
     private ArrayList<Sweet> sweets;

    public Gift() {
        sweets = new ArrayList<>();
    }

    public void addSweet(Sweet s) {
        sweets.add(s);
    }

    public void showGift() {
        for (Sweet s : sweets) {
            System.out.println(s);
        }
    }

    public double getTotalWeight() {
        double totalWeight = 0;
        for (Sweet s : sweets) {
            totalWeight += s.getWeight();
        }
        return totalWeight;
    }

    public void sortByCalories() {
        Collections.sort(sweets, Comparator.comparingDouble(Sweet::getCalories));
    }

    public void findBySugarRange(double minSugar, double maxSugar) {
        for (Sweet s : sweets) {
            if (s.getSugar() >= minSugar && s.getSugar() <= maxSugar) {
                System.out.println(s);
            }
        }
    }
    
}