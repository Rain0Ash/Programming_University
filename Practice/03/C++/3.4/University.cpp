#include <iostream>

bool try_get_input(double& first, int& second)
{
    std::cin >> first >> second;
    if (std::cin.fail())
    {
        std::cin.clear();
        std::cin.ignore(32767, '\n');
        return false;
    }

    std::cin.ignore(32767, '\n');
    return true;
}

void main()
{
    setlocale(LC_ALL, "Russian");

    double first;
    int second;

    while (!try_get_input(first, second))
    {
        std::cout << "Неверный ввод. Повторите попытку." << std::endl;
    }

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
