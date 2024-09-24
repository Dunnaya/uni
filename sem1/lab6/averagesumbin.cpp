/* 9. У бінарному файлі зберігається велика послідовність чисел. 
Вивести на екран середнє арифметичне всіх додатних елементів. */
#include <iostream>
#include <fstream>

using namespace std;

int main() 
{
    ifstream file("/Users/dunnaya/projects/helloworld/binaryfile.bin", ios::binary);

    if (!file.is_open()) 
    {
        cerr << "Failed to open file" << endl;
        return 1;
    }

    int num;
    int count = 0;
    long long sum = 0;

    while (file.read(reinterpret_cast<char*>(&num), sizeof(int)))
    {
        if (num > 0) 
        {
            sum += num;
            count++;
        }
    }

    file.close();

    if (count > 0) 
    {
        double average = static_cast<double>(sum) / count;
        cout << "Arithmetic average of positive numbers: " << average << endl;
    } else 
    {
        cout << "There are no positive numbers in the file" << endl;
    }

    return 0;
}
