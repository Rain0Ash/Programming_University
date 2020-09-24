#include <iostream>

bool try_get_input(int& x)
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

void try_read(int& value)
{
    while (!try_get_input(value))
    {
        std::cout << "Неверный ввод. Повторите попытку." << std::endl;
    }
}

void main()
{
    setlocale(LC_ALL, "Russian");

    std::cout << "Введите число." << std::endl;

    int value;

    do
    {
        try_read(value);

        if (value < 0)
        {
            std::cout << "Введеное значение является отрицательным. Повторите ввод." << std::endl;
        }
        else if (value > 20)
        {
            std::cout << "Введеное значение слишком велико. Введите иное значение." << std::endl;
        }

    } while (value < 0 || value > 20);

    long long unsigned int result = 1;

    for (int i = 2; i <= value; i++)
    {
        result *= i;
    }

    std::cout << result << std::endl;
}
