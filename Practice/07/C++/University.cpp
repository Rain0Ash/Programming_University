#include <iostream>

struct Point
{
    double X, Y;

    Point(double x, double y)
    {
        X = x;
        Y = y;
    }

    double distance(Point point)
    {
        return std::sqrt(std::pow(X - point.X, 2) + std::pow(Y - point.Y, 2));
    }
};

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

template<class T, class R>
bool try_get_input(T& x, R& y)
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

template<class T>
void try_read(T& value)
{
    while (!try_get_input(value))
    {
        std::cout << "Неверный ввод. Повторите попытку." << std::endl;
    }
}

template<class T, class R>
void try_read(T& x, R& y)
{
    while (!try_get_input(x, y))
    {
        std::cout << "Неверный ввод. Повторите попытку." << std::endl;
    }
}

Point try_read_point()
{
    double x, y;

    while (!try_get_input(x, y))
    {
        std::cout << "Неверный ввод. Повторите попытку." << std::endl;
    }

    return Point(x, y);
}

void main()
{
    setlocale(LC_ALL, "Russian");

    std::cout << "Введите метод решения." << std::endl;

    int method;
    do
    {
        try_read(method);

        if (method != 1 && method != 2)
        {
            std::cout << "Неверное значение метода. Повторите попытку ввода." << std::endl;
        }

    } while (method != 1 && method != 2);
    
    double a, b, c;

    switch (method)
    {
    case 1:
    {
        std::cout << "Введите число a" << std::endl;
        try_read(a);
        std::cout << "Введите число b" << std::endl;
        try_read(b);
        std::cout << "Введите число c" << std::endl;
        try_read(c);
        break;
    }
    case 2:
    {
        std::cout << "Введите точку A" << std::endl;
        Point A = try_read_point();
        std::cout << "Введите точку B" << std::endl;
        Point B = try_read_point();
        std::cout << "Введите точку C" << std::endl;
        Point C = try_read_point();

        a = A.distance(B);
        b = B.distance(C);
        c = C.distance(A);
        break;
    }
    default:
    {
        a = b = c = 0;
        std::cout << "Ошибка. Неверное значение метода." << std::endl;
        return;
    }
    }

    if (a + b <= c || a + c <= b || b + c <= a)
    {
        std::cout << "Треугольник не существует." << std::endl;
        return;
    }

    double p = (a + b + c) / 2;
    double s = std::sqrt(p * (p - a) * (p - b) * (p - c));

    std::cout << "S = " << s << std::endl;
}
