#include <iostream>

void main()
{
    setlocale(LC_ALL, "Russian");

    double first, second;
    std::cin >> first >> second;

    std::cout << std::endl;
    std::cout << "X + Y = " << first + second << std::endl;
    std::cout << "X - y = " << first - second << std::endl;
    std::cout << "Y - X = " << second - first << std::endl;
    std::cout << "X * Y = " << first * second << std::endl;
    
    std::cout << "X / Y = ";
    if (second != 0)
    {
        std::cout << first / second;
    }
    else
    {
        std::cout << "INF (Значение Y = 0)";
    }

    std::cout << std::endl;

    std::cout << "Y / X = ";
    if (first != 0)
    {
        std::cout << second / first;
    }
    else
    {
        std::cout << "INF (Значение X = 0)";
    }

    std::cout << std::endl;
}
