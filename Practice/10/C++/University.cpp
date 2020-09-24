#include <iostream>

bool try_get_input(long long int& s, long long int& l1, long long int& r1, long long int& l2, long long int& r2)
{
    std::cin >> s >> l1 >> r1 >> l2 >> r2;
    if (std::cin.fail())
    {
        std::cin.clear();
        std::cin.ignore(32767, '\n');
        return false;
    }

    std::cin.ignore(32767, '\n');
    return true;
}

void try_read(long long int& s, long long int& l1, long long int& r1, long long int& l2, long long int& r2)
{
    while (!try_get_input(s, l1, r1, l2, r2))
    {
        std::cout << "Неверный ввод. Повторите попытку." << std::endl;
    }
}

void main()
{
    setlocale(LC_ALL, "Russian");

    long long int s, l1, r1, l2, r2;

    try_read(s, l1, r1, l2, r2);

    if (l1 + r2 >= s)
    {
        r2 = std::max(s - l1, l2);
    }
    else
    {
        l1 = std::min(s - r2, r1);
    }

    if (l1 + r2 == s)
    {
        std::cout << l1 << ' ' << r2 << std::endl;
    }
    else
    {
        std::cout << -1 << std::endl;
    }
}
