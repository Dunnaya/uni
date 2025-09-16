package com.lab1.sweets;

public class Chocolate extends Sweet 
{
    private String cocoaPercentage;
    private boolean hasNuts;
    public enum ChocolateType { MILK, DARK, WHITE }
    private ChocolateType type;

    public Chocolate(String name, double weight, double calories, double sugar, String cocoaPercentage, boolean hasNuts, ChocolateType type) 
    {
        super(name, weight, calories, sugar);
        this.cocoaPercentage = cocoaPercentage;
        this.hasNuts = hasNuts;
        this.type = type;
    }

    public String getCocoaPercentage() { return cocoaPercentage; }
    public boolean hasNuts() { return hasNuts; }
    public ChocolateType getType() { return type; }

    @Override
    public String toString() 
    {
        return super.toString() + ", Cocoa: " + cocoaPercentage + ", Contains Nuts: " + hasNuts + ", Type: " + type;
    }
}
