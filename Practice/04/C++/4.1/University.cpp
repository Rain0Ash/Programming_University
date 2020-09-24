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

    int temp;
    int a;
    int b;

    while (!try_get_input(a, b))
    {
        std::cout << "Неверный ввод. Повторите попытку." << std::endl;
    }

    // Тут можно провести оптимизацию сравнив a и b и не обменивая их значениями в случае эквивалентности их значений.

    temp = a;
    a = b;
    b = temp;
    
    std::cout << a << ' ' << b << std::endl;
}
