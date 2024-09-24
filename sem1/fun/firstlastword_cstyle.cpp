/* Написати функцію, яка для рядка (рядок в «стилі С») перевіряє чи співпадає його перше слово з останнім. */
#include <iostream>
#include <cstring>

using namespace std;

bool firstLastWord(const char lett[])
{
    int len = strlen (lett);
    int i = len;
    while (i >= 0 && lett[i] != ' ') 
    {
        i--;
    }
    int j = 0;
    i++;
    while(j < i && lett[j] != ' ' && i < len)
    {
        if (lett[i] != lett[j]) 
            return false;
        i++;
        j++;
    }

    if(lett[j] == ' ' && i == len) return true;
    return false;

}

int main()
{
    char lett[255];
    cin.getline(lett, 255);

    if (firstLastWord(lett)) cout << "YESSSS";
    else cout << "NOOOOOO";
}
