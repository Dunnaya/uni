/*Реалізувати дві структури, що описують точку на площині у декартових та полярних координатах
відповідно. Операції: ввід, вивід, перевід з полярних координат у декартові та навпаки.*/

#include <iostream>
#include <cmath>

using namespace std;

struct CartesianPoint 
{
    double x;
    double y;
};

struct PolarPoint 
{
    double radius;
    double angle;
};

// перетворення полярних у декартові
CartesianPoint polarToCartesian(const PolarPoint& polar) 
{
    double x = polar.radius * cos(polar.angle);
    double y = polar.radius * sin(polar.angle);

    return {x, y};
}

// перетворення декартових у полярні
PolarPoint cartesianToPolar(const CartesianPoint& cartesian) 
{
    double radius = sqrt(cartesian.x * cartesian.x + cartesian.y * cartesian.y);
    double angle = atan2(cartesian.y, cartesian.x);

    return {radius, angle};
}

void printCartesianPoint(const CartesianPoint& point) 
{
    cout << "Декартові координати: (" << point.x << ", " << point.y << ")" << endl;
}

void printPolarPoint(const PolarPoint& point) 
{
    cout << "Полярні координати: (r = " << point.radius << ", φ = " << point.angle << " рад)" << endl;
}

int main() 
{
    CartesianPoint cartesianPoint;
    PolarPoint polarPoint;

    cout << "Введіть координати точки у декартових координатах (x y): ";
    cin >> cartesianPoint.x >> cartesianPoint.y;

    printCartesianPoint(cartesianPoint);
    polarPoint = cartesianToPolar(cartesianPoint);
    printPolarPoint(polarPoint);

    cout << "Введіть координати точки у полярних координатах (r θ в радіанах): ";
    cin >> polarPoint.radius >> polarPoint.angle;

    printPolarPoint(polarPoint);
    cartesianPoint = polarToCartesian(polarPoint);
    printCartesianPoint(cartesianPoint);

    return 0;
}
