package com.lab1.sweets;

public abstract class Sweet 
{
    private String name;
    private double weight;
    private double calories; // kcal
    private double sugar; // all in grams

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
        return name + " (Weight: " + weight + "g, Calories: " + calories + "kcal, Sugar: " + sugar + "g)";
    }
}