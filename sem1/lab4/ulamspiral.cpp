// Ulam spiral
#include <iostream>
#include <cmath>

using namespace std;

bool isPrime(int n);

int main()
{
    const int SIDE = 9;
    int arr[SIDE][SIDE];
    int num = SIDE * SIDE;

    int top = 0;
    int bottom = SIDE - 1;
    int left = 0;
    int right = SIDE - 1;

    while (top <= bottom && left <= right)
    {
        for (int i = right; i >= left; i--)
        {
            if (isPrime(num))
            {
                arr[bottom][i] = num;
            }
            else arr[bottom][i] = 0;
            num--;
        }
        bottom--;

        for (int i = bottom; i >= top; i--)
        {
            if (isPrime(num))
            {
                arr[i][left] = num;
            }
            else arr[i][left] = -1;
            num--;
        }
        left++;

        for (int i = left; i <= right; i++)
        {
            if (isPrime(num))
            {
                arr[top][i] = num;
            }
            else arr[top][i] = -2;
            num--;
        }
        top++;

        for (int i = top; i <= bottom; i++)
        {
            if (isPrime(num))
            {
                arr[i][right] = num;
            }
            else arr[i][right] = -3;
            num--;
        }
        right--;
    }

    for (int i = 0; i < SIDE; i++)
    {
        for (int j = 0; j < SIDE; j++)
        {
            if (arr[i][j] == 0) cout << "->\t";
            else if (arr[i][j] == -1) cout << "v" << "\t";
            else if (arr[i][j] == -2) cout << "<-\t";
            else if (arr[i][j] == -3) cout << "^" << "\t";
            else cout << arr[i][j] << "\t";
        }
        cout << "\n\n\n";
    }

    return 0;
}

bool isPrime(int n)
{
    if (n == 1) return false;
    if (n % 2 == 0 && n != 2)
    {
        return false;
    }

    int div = 3;

    while (div <= sqrt(n))
    {
        if (n % div == 0)
        {
            return false;
        }
        div += 2;
    }

    return true;
}
