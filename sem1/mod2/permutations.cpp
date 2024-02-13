/*Користувач вводить два числа: n та k. Написати рекурсивну функцію для виводу на екран всіх пере-
становок множини 1,2,...,n, у яких для кожних двох сусідніх елементів виконується |ai − ai+1| <= k
∀i ∈ 1, 2, ..., n − 1*/

#include <iostream>
#include <vector>

using namespace std;

void displayPermutations(vector<int>& permutation) 
{
    for (int i = 0; i < permutation.size(); ++i) 
    {
        cout << permutation[i] << " ";
    }
    cout << endl;
}

//|ai − ai+1| <= k
bool isOk(const vector<int>& permutation, int k) 
{
    for (int i = 0; i < permutation.size() - 1; ++i) //для останнього елемента немає наступного
    {
        if (abs(permutation[i] - permutation[i + 1]) > k) 
        {
            return false;
        }
    }
    return true;
}

void generatePermutations(vector<int>& permutation, int n, int k) 
{
    if (permutation.size() == n) 
    {
        if (isOk(permutation, k)) 
        {
            displayPermutations(permutation);
        }
        return;
    }

    for (int i = 1; i <= n; ++i) 
    {
        if (find(permutation.begin(), permutation.end(), i) == permutation.end()) 
        {
            permutation.push_back(i);                   // +i до кінця вектора permutation
            generatePermutations(permutation, n, k);
            permutation.pop_back();                     //видалення останнього елемента з кінця вектора
        }
    }
}

int main() 
{
    int n, k;
    
    cout << "Enter two numbers: n and k: ";
    cin >> n >> k;

    vector<int> permutation;
    generatePermutations(permutation, n, k);

    return 0;
}
