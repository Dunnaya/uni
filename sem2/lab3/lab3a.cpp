/* Координати точок в тривимірному просторі (дійсні), сортування за x-y-z, за спаданням
Selection sort
Hoare partition scheme, rightmost pivot
bottom-up */

#include <iostream>
#include <vector>
#include <algorithm>
using namespace std;

struct point 
{
    double x, y, z;

    point (double x, double y, double z) 
    {
        this -> x = x, 
        this -> y = y, 
        this -> z = z;
    };

    void returnCoordinate ()
    {
        cout << "(" << x << ";" << y << ";" << z << ")";
    }

    void returnCoordinateBenchmark ()
    {
        for (int a = 0; a <= 7; a++) {} // for the same number of commands
    }

    point () {};
};

/*void xSortBubble(vector <point> points)
{
    for(int i = 0; i < (points.size() - 1); i++)
    {
        for(int j = 0; j < (points.size() - 1); j++)
        {
            if(points[j].x < points[j+1].x)
            {
                swap(points[j], points[j+1]);
            }
        }
    }
}*/

void xSelectionSort(vector <point> points)
{
    for(int i = 0; i < (points.size() - 2); i++)
    {
        int indexmax = i;

        for(int j = i + 1; j < (points.size() - 1); j++)
        {
            if(points[j].x > points[indexmax].x)
            {
                indexmax = j;
            }
        }

        if(indexmax != i)
        {
            swap(points[i], points[indexmax]);
        }
    }
}

/*void ySortBubble(vector <point> points)
{
    for(int i = 0; i < (points.size() - 1); i++)
    {
        for(int j = 0; j < (points.size() - 1); j++)
        {
            if(points[j].y < points[j+1].y)
            {
                swap(points[j], points[j+1]);
            }
        }
    }
}*/

void ySelectionSort(vector <point> points)
{
    for(int i = 0; i < (points.size() - 2); i++)
    {
        int indexmax = i;

        for(int j = i + 1; j < (points.size() - 1); j++)
        {
            if(points[j].y > points[indexmax].y)
            {
                indexmax = j;
            }
        }

        if(indexmax != i)
        {
            swap(points[i], points[indexmax]);
        }
    }
}

/*void zSortBubble(vector <point> points)
{
    for(int i = 0; i < (points.size() - 1); i++)
    {
        for(int j = 0; j < (points.size() - 1); j++)
        {
            if(points[j].z < points[j+1].z)
            {
                swap(points[j], points[j+1]);
            }
        }
    }
}*/

void zSelectionSort(vector <point> points)
{
    for(int i = 0; i < (points.size() - 2); i++)
    {
        int indexmax = i;

        for(int j = i + 1; j < (points.size() - 1); j++)
        {
            if(points[j].z > points[indexmax].z)
            {
                indexmax = j;
            }
        }

        if(indexmax != i)
        {
            swap(points[i], points[indexmax]);
        }
    }
}

void demoMode() 
{

}

void benchmark() 
{

}

int main () 
{
    int choice;

    cout << "Modes:\n";
    cout << "1. Demo\n";
    cout << "2. Benchmark\n";
    cout << "Select mode: ";
    cin >> choice;

    switch(choice)
    {
        case 1:
        {
            demoMode();

            break;
        }

        case 2:
        {
            benchmark();

            break;
        }

        default:
        cout << "Invalid choice.\n";
        break;
    }
    return 0;
}
