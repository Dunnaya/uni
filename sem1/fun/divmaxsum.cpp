/*1. Написати функцію для знаходження натурального числа, що не  перевищує N, з максимальною сумою власних дільників.*/

#include <iostream>
#include <cmath>
using namespace std;

int nDivMaxSum(int N) 
{
    int maxSum = 0;
    int resN = 0;

    for (int n = 1; n <= N; ++n)
    {
        int divSum = 0;
        for (int div = 1; div <= sqrt(n); ++div)
        {
            if (n % div == 0)
            {
                divSum += div;

                if (n / div != div)
                    divSum += n / div;
            }
        }
        if (divSum > maxSum)
        {
            maxSum = divSum;
            resN = n;
        }
    }
    
    return resN;
}
int main()
{
    int N;
    cout << "Enter N: ";
    cin >> N;
    int res = nDivMaxSum(N);
    cout << res;

    return 0;
}
