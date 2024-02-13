#include <iostream>
#include <fstream>
#include <cstdlib>

using namespace std;

int main() 
{
    ofstream file("/Users/dunnaya/projects/helloworld/binary_file.bin", ios::binary);

    if (!file.is_open()) 
    {
        cerr << "Could not open file for writing" << endl;
        return 1;
    }

    for (int i = 0; i < 100; ++i) 
    {
        int randomNum = rand() % 100 - 50; // видає випадкове число від -50 до 49
        file.write(reinterpret_cast<const char*>(&randomNum), sizeof(int));
    }

    file.close();

    cout << "Success!!!!!!! File was created" << endl;

    return 0;
}
