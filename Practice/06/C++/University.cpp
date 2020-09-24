#include <iostream>

bool try_get_input(double& x)
{
    std::cin >> x;
    if (std::cin.fail())
    {
        std::cin.clear();
        std::cin.ignore(32767, '\n');
        return false;
    }

    std::cin.ignore(32767, '\n');
    return true;
}

void try_read(double& x)
{
    while (!try_get_input(x))
    {
        std::cout << "Неверный ввод. Повторите попытку." << std::endl;
    }
}

void main()
{
    setlocale(LC_ALL, "Russian");

    double a, b, c;

    std::cout << "Введите число a" << std::endl;
    try_read(a);
    std::cout << "Введите число b" << std::endl;
    try_read(b);
    std::cout << "Введите число c" << std::endl;
    try_read(c);

    if (a == 0) //если а == 0 то решаем линейное уравнение
    {
        if (b == 0 && c == 0)
        {
            std::cout << "Бесконечное количество решений" << std::endl;
        }
        else if (b != 0 && (c == 0 || abs(b) <= abs(c)) && std::fmod(c, b) == 0)
        {
            std::cout << "x = " << -c / b << std::endl;
        }
        else
        {
            std::cout << "Корней нет." << std::endl;
        }

        return;
    }

    double d = b * b - 4 * a * c;

    if (d > 0)
    {
        std::cout << "x1 = " << (-b + std::sqrt(d)) / (2 * a) << std::endl;
        std::cout << "x2 = " << (-b - std::sqrt(d)) / (2 * a) << std::endl;
    }
    else if (d == 0)
    {
        std::cout << "x = " << -b / (2 * a) << std::endl;
    }
    else
    {
        std::cout << "Действительных корней нет." << std::endl;
    }
}
