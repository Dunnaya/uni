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
    std::string T = "bca";
    std::string T_star = "abc";

    std::string T1 = "xyz";

    std::string T2 = "ab";

    if (isCyclicShift(T, T_star))
       std::cout << "true\n";
    else
       std::cout << "false\n";

    if (isCyclicShift(T1, T_star))
        std::cout << "true\n";
    else
        std::cout << "false\n";

    if (isCyclicShift(T2, T_star))
        std::cout << "true\n";
    else
        std::cout << "false\n";

    return 0;
}