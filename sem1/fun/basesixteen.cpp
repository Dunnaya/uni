/* Написати функцію, яка для цілого числа знаходить кількість різних цифр
у представленні цього числа за основою 16. */
#include <iostream>

using namespace std;

int sixteenmissedcalls(int n)
{
    bool isNum [10] = {0};
    int temp, count=0;
    while (n != 0)
    {
        temp = n % 16;
        if (temp < 10 && !isNum [temp])
        {
            count++;
            isNum[temp] = true;
        }
        n /= 16;
    }
    return count;
}

int main()
{
    int n;
    cin >> n;
    cout << sixteenmissedcalls(n);
}
