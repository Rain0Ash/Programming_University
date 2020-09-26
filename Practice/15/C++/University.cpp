#include <iostream>
#include <time.h>

bool try_get_input(int& value)
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

void try_read(int& value)
{
    while (!try_get_input(value))
    {
        std::cout << "Неверный ввод. Повторите попытку." << std::endl;
    }
}

void main()
{
    setlocale(LC_ALL, "Russian");

    srand(time(NULL));

    do
    {
        int result = rand() % 3; //От 0 до 100
        int count = 5;

        std::cout << "Угадай число за 5 попыток." << std::endl;

        do
        {
            int value;
            try_read(value);

            if (value == result)
            {
                std::cout << "Поздравляю! Вы угадали" << std::endl;
                break;
            }
            else if (value > result)
            {
                std::cout << "Загаданное число меньше" << std::endl;
            }
            else if (value < result)
            {
                std::cout << "Загаданное число больше" << std::endl;
            }

        } while (--count > 0);
        
        if (count == 0)
        {
            std::cout << "Вы проиграли. Было загадано: " << result << std::endl;
        }
        
        std::cout << "Хотите начать сначала? (1 - ДА )" << std::endl;

        int restart;
        try_read(restart);

        if (restart != 1)
        {
            break;
        }

    } while (true);
}
