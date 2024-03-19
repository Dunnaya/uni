#include <iostream>
#include <vector>
#include <chrono>
using namespace std;

void insertionSort(vector<double>& bucket) 
{
    for (int i = 1; i < bucket.size(); ++i) 
    {
        double key = bucket[i];
        int j = i - 1;
        while (j >= 0 && bucket[j] > key) 
        {
            bucket[j + 1] = bucket[j];
            j--;
        }
        bucket[j + 1] = key;
    }
}

void bucketSort( vector<double>& arr, int n) 
{
    vector<vector<double> > b(n);

    for (int i = 0; i < n; i++) 
    {
        int bi = static_cast<int>(n * arr[i]);
        b[bi].push_back(arr[i]);
    }

    for (int i = 0; i < n; i++) 
    {
        insertionSort(b[i]);
    }

    int index = 0;
    for (int i = 0; i < n; i++) 
    {
        for (int j = 0; j < b[i].size(); j++) 
        {
            arr[index++] = b[i][j];
        }
    }
}

void demoMode()
{
    int n = 10;
    vector<double> numbers;
    numbers.push_back(0.11);
    numbers.push_back(0.45);
    numbers.push_back(0.194);
    numbers.push_back(0.678);
    numbers.push_back(0.673);
    numbers.push_back(0.3);
    numbers.push_back(0.578);
    numbers.push_back(0.906);
    numbers.push_back(0.1);
    numbers.push_back(0.167);

    cout << "Numbers: ";
    for(auto& number : numbers)
    {
        cout << number << " ";
    }
    cout << endl;

    bucketSort(numbers, n);

    cout << "Numbers sorted: ";
    for(auto& number : numbers)
    {
        cout << number << ", ";
    }
}

void benchmark()
{
    int n = 0;
    cout << "Enter N: ";
    cin >> n;
    vector<double> numbers;

    for(int i = 0; i < n; i++)
    {
        double number = rand()% 1000 / 100.0;
        numbers.push_back(number);
    }

    auto startSort = chrono::high_resolution_clock::now();
    bucketSort(numbers, n);
    auto endSort = chrono::high_resolution_clock::now();

    auto durationSort = chrono::duration_cast<chrono::milliseconds>(endSort - startSort);

    cout << "Sort time: " << durationSort.count() << endl;
}

int main() 
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
    }
    return 0;

}
