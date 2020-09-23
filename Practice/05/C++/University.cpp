#include <iostream>

bool try_get_input(double& x0, double& v0, double& t)
{
    std::cin >> x0 >> v0 >> t;
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
    const double g = 9.8;
    const double a = g;

    setlocale(LC_ALL, "Russian");

    double x0, v0, t;
    //Выбираем double, т.к. математически формула способна работать с нецелочисленными типами,
    //а с современными мощностями и количеством памяти выбирать между double и float в данном случае не имеет смысла.

    while (!try_get_input(x0, v0, t))
    {
        std::cout << "Неверный ввод. Повторите попытку." << std::endl;
    }

    std::cout << std::abs(x0 + v0*t - a*t*t/2) << std::endl; //Используем функцию abs (модуль числа), так как нам надо получить пройденное расстояние.

    //Да, разница в написании формулы существует. 1/2 математически равное 0.5, в данном случае преобразуется в int(1)/int(2), которое преобразуется в int(1) div int(2), что равно 0, и, следовательно, последняя часть формулы обращается в 0.
}
