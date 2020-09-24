#include <iostream>

bool try_get_input(double& x, int& y)
{
    std::cin >> x >> y;
    if (std::cin.fail())
    {
        std::cin.clear();
        std::cin.ignore(32767, '\n');
        return false;
    }

    std::cin.ignore(32767, '\n');
    return true;
}

void try_read(double &x, int& y)
{
    while (!try_get_input(x, y))
    {
        std::cout << "Неверный ввод. Повторите попытку." << std::endl;
    }
}

void main()
{
    setlocale(LC_ALL, "Russian");

    double x;
    int y;

    double result;

    std::cout << "Введите числа x и y" << std::endl;

    try_read(x, y);

    if (y == 0 || x == 1)
    {
        result = 1;
    }
    else if (y == 1)
    {
        result = x;
    }
    else if (x == 0)
    {
        result = 0;
    }
    else
    {
        bool negative = y < 0;
        if (negative)
        {
            y = -y;
        }
       
        result = x;

        for (int i = 1; i < y; ++i)
        {
            result *= x;
        }

        if (negative)
        {
            result = 1 / result;
        }
    }
    
    std::cout << result << std::endl;
}