/*Реалізувати В+ - дерево*/
#include <iostream>
#include <ostream>
#include <vector>
using namespace std;

struct Complex 
{
    double real;
    double imaginary;

    Complex(double r, double i) : real(r), imaginary(i) {}

    Complex operator+(const Complex& other) const 
    {
        return Complex(real + other.real, imaginary + other.imaginary);
    }

    bool operator<(const Complex& other) const 
    {
        return sqrt(real * real + imaginary * imaginary) < sqrt(other.real * other.real + other.imaginary * other.imaginary);
    }

    bool operator==(const Complex& other) const 
    {
        return real == other.real && imaginary == other.imaginary;
    }

    friend ostream& operator<<(ostream& os, const Complex& c) 
    {
        os << "(" << c.real << ", " << c.imaginary << ")";
        return os;
    }
};

int main() {
    return 0;
}