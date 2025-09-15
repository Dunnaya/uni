package com.lab1.sweets;

public class Candy extends Sweet 
{
    private String flavor;
    private boolean isHard;

    public Candy(String name, double weight, double calories, double sugar, String flavor, boolean isHard) 
    {
        super(name, weight, calories, sugar);
        this.flavor = flavor;
        this.isHard = isHard;
    }

    public String getFlavor() { return flavor; }
    public boolean isHardCandy() { return isHard; }

    @Override
    public String toString() 
    {
        return super.toString() + ", Flavor: " + flavor + ", Hard Candy: " + isHard;
    }   
}