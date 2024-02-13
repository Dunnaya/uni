// Визначити потрібні типи даних й написати функцію sum(a, b), що здійснює додавання двох раціональних чисел a та b.
#include <iostream>
using namespace std;

struct rational
{
    int numerator;
    int denominator;
};

int findNSD (int a, int b) // алгоритм евкліда
{
    while (b != 0)
    {
        int temp = b;
        b = a % b;
        a = temp;
    }
    return a;
}

rational simplify (rational num)
{
    int nsd = findNSD(num.numerator, num.denominator);
    num.numerator /= nsd;
    num.denominator /= nsd;
    return num;
}

rational sum (rational a, rational b)
{
    rational res;
    res.numerator = a.numerator * b.denominator   +   b.numerator * a.denominator;
    res.denominator = a.denominator * b.denominator;

    res = simplify (res);
    return res;
}

int main() 
{
    rational a = {1, 2};     // 1/2
    rational b = {2, 3};     // 2/3

    rational result = sum(a, b);

    cout << "Сума: " << result.numerator << "/" << result.denominator << endl;

    return 0;
}
