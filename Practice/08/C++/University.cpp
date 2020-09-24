#include <iostream>

bool try_get_input(double& x, char& operation, double& y)
{
    std::cin >> x >> operation >> y;
    if (std::cin.fail())
    {
        std::cin.clear();
        std::cin.ignore(32767, '\n');
        return false;
    }

    std::cin.ignore(32767, '\n');
    return true;
}

void try_read(double& x, char& operation, double& y)
{
    while (!try_get_input(x, operation, y))
    {
        std::cout << "Неверный ввод. Повторите попытку." << std::endl;
    }
}

bool check_operation(char operation)
{
    return operation == '+' || operation == '-' || operation == '*' || operation == '/';
}

void main()
{
    setlocale(LC_ALL, "Russian");

    std::cout << "Введите число1 (+-*/) число2" << std::endl;

    double x, y;
    char operation;

    do
    {
        try_read(x, operation, y);

        if (!check_operation(operation))
        {
            std::cout << "Неверная операция. Повторите ввод." << std::endl;
        }

    } while (!check_operation(operation));

    double value;

    switch (operation)
    {
    case '+':
        value = x + y;
        break;
    case '-':
        value = x - y;
        break;
    case '*':
        value = x * y;
        break;
    case '/':
        value = x / y;
        break;
    default:
    {
        std::cout << "Неверная операция." << std::endl;
        return;
    }
    }

    std::cout << value << std::endl;
}
