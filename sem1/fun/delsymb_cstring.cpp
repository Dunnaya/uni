/*Написати функцію, що вилучає з першого заданого символьного рядка V всі символи, 
які належать другому заданому рядку W (рядки в «стилі С»).*/

#include <iostream>
#include <cstring>

using namespace std;

void delSymb(char V[], const char W[]) 
{
    int lenV = strlen(V);
    int lenW = strlen(W);

    for (int i = 0; i < lenV; ++i) 
    {
        for (int j = 0; j < lenW; ++j) 
        {
            if (V[i] == W[j]) 
            {   // якщо знайдено спільний символ, вилучаємо його
                for (int k = i; k < lenV - 1; ++k) 
                {
                    V[k] = V[k + 1];
                }
                V[lenV - 1] = '\0';
                --i;
                break;
            }
        }
    }
}

int main() {
    const int Розмір_V = 100;
    char V[Розмір_V];
    char W[Розмір_V];

    cout << "Введіть рядок V: ";
    cin.getline(V, Розмір_V);

    cout << "Введіть рядок W: ";
    cin.getline(W, Розмір_V);

    delSymb(V, W);

    cout << "Результат: " << V << endl;

    return 0;
}
