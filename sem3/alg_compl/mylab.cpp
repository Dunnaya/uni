/*Розробіть алгоритм, який за лінійний час визначав би, чи є текстовий
рядок Т циклічним зсувом іншого рядка Т* (наприклад, abc та cab).*/
#include <iostream>
#include <string>

bool isCyclicShift(std::string T, std::string T_star)
{
    std::string S;

    if(T.length() != T_star.length())
        return false; //if they have diff length cyclic shift is impossible
    
    S = T_star + T_star; 

    return S.find(T) != std::string::npos;
}

int main()
{
    std::string str1, str2;
    char choice;

    do 
    {
        std::cout << "Enter the 1st line: ";
        std::getline(std::cin, str1);

        std::cout << "Enter the 2nd line: ";
        std::getline(std::cin, str2);

        isCyclicShift(str1, str2);

        if (isCyclicShift(str1, str2))
        std::cout << "The 1st line IS cyclic shift of the 2nd line.\n";
        else
        std::cout << "The 1st line IS NOT cyclic shift of the 2nd line.\n";

        std::cout << "Do you want to check another pair of lines? (y/n): ";
        std::cin >> choice;
        std::cin.ignore();

    } while(choice == 'y' || choice == 'Y');

    std::cout << "Exiting the program...\n";
    
    return 0;
}