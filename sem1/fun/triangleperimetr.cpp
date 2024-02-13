/* Визначити потрібні типи даних й написати функцію, яка серед заданих n точок на площині
обирає три з них, щоб периметр трикутника у обраних точках був найбільшим. */

#include <iostream>
#include <cmath>
#include <cstdlib>
using namespace std;
struct point
{
    int x;
    int y;
};
int perimetr (point a, point b, point c)
{
    int s1 = sqrt((a.x-b.x)*(a.x-b.x) - (a.y-b.y)*(a.y-b.y));
    int s2 = sqrt((b.x-c.x)*(b.x-c.x) - (b.y-c.y)*(b.y-c.y));
    int s3 = sqrt((a.x-c.x)*(a.x-c.x) - (a.y-c.y)*(a.y-c.y));
    return s1 + s2 + s3;
}
int hueta(int n, point * mas)
{
    int etalon = 0, temp = 0;
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n; j++)
        {
            for (int k = 0; k < n; k++)
            {
                temp = perimetr(mas[i], mas[j], mas[k]);
                if(etalon < temp) etalon = temp;
            }
        }
    }
    return etalon;
}

int main()
{
    int n;
    point mas[100];
    cin >> n;
    cout << "Point: ";
    for(int i = 0; i < n; i++)
    {
        mas[i].x = rand() % 100;
        mas[i].y = rand() % 100;
        cout << mas[i].x << " " << mas[i].y << "\n";
    }
    cout << "Preimetrs: ";
    cout << hueta(n, mas);

}
