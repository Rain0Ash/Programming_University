#include <iostream>

bool try_get_input(int& a, int& b)
{
    std::cin >> a >> b;
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

    int a;
    int b;

    while (!try_get_input(a, b))
    {
        std::cout << "Неверный ввод. Повторите попытку." << std::endl;
    }

    b = a + b;
    a = b - a;
    b = b - a;
    
    std::cout << a << ' ' << b << std::endl;
}
