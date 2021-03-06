#include <iostream>

using namespace std;

int* create(int length, int start = 0, int step = 0)
{
    int* array = new int[length];

    for (int i = 0; i < length; i++)
    {
        array[i] = start;
        start += step;
    }

    return array;
}

void create(int** array, int length, int start, int step)
{
    *array = create(length, start, step);
}

void destroy(int** array)
{
    if (array == nullptr)
    {
        return;
    }

    delete[] *array;
    *array = nullptr;
}

void sort(int* array, int size)
{
    for (int i = 1; i < size; i++)
    {
        int temp = array[i];
        int item = i - 1;
        while(item >= 0 && array[item] > temp)
        {
            array[item + 1] = array[item];
            array[item] = temp;
            item--;
        }
    }
}

int* print(int* array, int size)
{
    std::cout << '[';
    for (int i = 0; i < size; i++)
    {
        std::cout << array[i];

        if (i < size - 1)
        {
            std::cout <<", ";
        }
    }

    std::cout << ']' << std::endl;

    return array;
}

int main()
{
    int length, start, step;

    std::cin >> length >> start >> step;

    int* array;
    create(&array, length, start, step);
    sort(array, length);
    print(array, length);
    destroy(&array);

    return 0;
}
