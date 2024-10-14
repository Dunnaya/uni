/*1. Натуральне число називають досконалим, якщо воно дорівнює сумі всіх своїх власних 
дільників, включаючи 1. Написати функцію для знаходження всіх досконалих чисел, що менші за N.*/
#include <iostream>

using namespace std;

bool isPerfectNumber(int num) 
{
    int sum = 1;
    
    for (int i = 2; i <= num / 2; i++) 
    {
        if (num % i == 0) 
        {
            sum += i;
        }
    }
    
    return sum == num;
}

int main() {
    int n;
    cout << "Введіть n: ";
    cin >> n;
    cout << "Досконалі числа не більші за " << n << ":" << endl;

    for (int i = 2; i <= n; i++) 
    {
        if (isPerfectNumber(i)) 
        {
            cout << i << " ";
        }
    }
    
    cout << endl;

    return 0;
}
