// Вирази задаються у вигляді функцій, які передаються як параметр.

#include <iostream>

#include <cmath>

using namespace std;

double func(double fi) 
{
    // тут можна ввести формулу для r(ϕ) відповідно до задачі
    return fi; // r(fi) = 2.0 для кола з радіусом 2
}

double integral(double (*r)(double), double a, double b) // double (*r)(double): визначення параметра r; передаємо функцію як параметр
{
    double sectorArea = 0.0;
    int n = 1000;
    double st = (b - a) / n;

    for (int i = 0; i < n; i++) 
    {
        double fi = a + i * st; //fi представляє кут, який змінюється від a до b і використ. для обчислення r(fi)
        double r_fi = r(fi);
        sectorArea += 0.5 * r_fi * r_fi * st; 
    }
    
    sectorArea = abs(sectorArea);
    return sectorArea;
}

int main() 
{
    double a, b;

    cout << "Enter the starting angle (in radians): ";
    cin >> a;

    cout << "Enter the end angle (in radians): ";
    cin >> b;

    cout << "Sector area: " << integral (func, a, b) << endl;

    return 0;
}
