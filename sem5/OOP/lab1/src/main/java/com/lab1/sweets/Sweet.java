package com.lab1.sweets;

public abstract class Sweet 
{
    private String name;
    private double weight; // for 1 piece in grams
    private double calories; // kcal/100g
    private double sugar; // all in grams /100g

    public Sweet(String name, double weight, double calories, double sugar) 
    {
        this.name = name;
        this.weight = weight;
        this.calories = calories;
        this.sugar = sugar;
    }

    public String getName() { return name; }
    public double getWeight() { return weight; }
    public double getCalories() { return calories; }
    public double getSugar() { return sugar; }

    @Override
    public String toString() 
    {
        return name + " (Weight: " + weight + "g, Calories: " + calories + " kcal/100g, Sugar: " + sugar + "g)";
    }
}