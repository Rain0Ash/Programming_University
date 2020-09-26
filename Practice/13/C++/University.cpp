#include <iostream>

template<class T>
bool try_get_input(T& value)
{
    std::cin >> value;
    if (std::cin.fail())
    {
        std::cin.clear();
        std::cin.ignore(32767, '\n');
        return false;
    }

    std::cin.ignore(32767, '\n');
    return true;
}

template<class T>
void try_read(T& value)
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

    try_read(value);

    if (value < 2)
    {
        std::cout << "Число меньше двух" << std::endl;
    }
    else if (value == 2) //Проводим некоторые оптимизации, в частности с числом 2
    {
        std::cout << "Простое" << std::endl;
    }
    else if (value % 2 == 0)
    {
        std::cout << "Составное" << std::endl;
    }
    else
    {
        int divisor = 3;

        while (divisor * divisor <= value && value % divisor != 0)
        {
            divisor += 2;
        }

        std::cout << (divisor * divisor <= value ? "Составное" : "Простое") << std::endl;
    }
}
