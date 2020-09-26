#include <iostream>
#include <vector>

bool try_get_input(char*& string)
{
    std::cin >> string;
    if (std::cin.fail())
    {
        std::cin.clear();
        std::cin.ignore(32767, '\n');
        return false;
    }

    std::cin.ignore(32767, '\n');
    return true;
}

void try_read(int& time)
{
    char* string = (char*)malloc(INT16_MAX); //выделяем память, выступающую в роли string. 32767 - значение Int16.MaxValue, которое также является длиной строки консоли.
    int hours, minutes;

    bool successful;
    do
    {
        successful = try_get_input(string);
        
        if (sscanf_s(string, "%d:%d", &hours, &minutes) == 2) //проверяем, полностью ли запарсены переменные hours и minutes
        {
            if (hours < 0 || hours > 23 || minutes < 0 || minutes > 59)
            {
                std::cout << "Неверное время. Повторите ввод." << std::endl;
                successful = false;
            }
            else
            {
                time = hours * 60 + minutes;
            }
        }
        else
        {
            std::cout << "Неверный ввод. Повторите попытку." << std::endl;
            successful = false;
        }

    } while (!successful);

    free(string);
}

void main()
{
    setlocale(LC_ALL, "Russian");

    int first, second;

    try_read(first);
    try_read(second);

    std::cout << (abs(second - first) <= 15 ? "Встреча состоится" : "Встреча не состоится") << std::endl;
}
