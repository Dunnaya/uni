package com.lab1.sweets;

public class Jelly extends Sweet 
{
    private String shape;
    private boolean isSour;

    public Jelly(String name, double weight, double calories, double sugar, String shape, boolean isSour) 
    {
        super(name, weight, calories, sugar);
        this.shape = shape;
        this.isSour = isSour;
    }

    public String getShape() { return shape; }
    public boolean isSourCandy() { return isSour; }

    @Override
    public String toString() 
    {
        return super.toString() + ", Shape: " + shape + ", Sour: " + isSour;
    }
}
