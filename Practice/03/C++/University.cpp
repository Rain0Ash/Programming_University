#include <iostream>

template<class T, class R>
bool try_get_input(T& first, R& second)
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

template<class T, class R>
void execute(T& first, R& second)
{
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

void main()
{
    setlocale(LC_ALL, "Russian");

    //

    int if1, if2;

    std::cout << "Введите два целочисленных числа" << std::endl;

    execute(if1, if2);
    
    //

    double ds1, ds2;

    std::cout << "Введите два вещественных числа" << std::endl;

    execute(ds1, ds2);
    
    //

    int it1;
    double dt2;

    std::cout << "Введите целочисленное и вещественное число" << std::endl;

    execute(it1, dt2);
    
    //

    double dq1;
    int iq2;

    std::cout << "Введите вещественное и целочисленное число" << std::endl;

    execute(dq1, iq2);
}
