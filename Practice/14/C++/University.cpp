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

    long long unsigned int value;

    try_read(value);

    std::cout << (value <= 0 ? 0 : (long long unsigned int) std::log2l((long double) value) + 1) << std::endl;
}
