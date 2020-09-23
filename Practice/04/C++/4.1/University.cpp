#include <iostream>

void main()
{
    int temp;
    int a;
    int b;

    std::cin >> a >> b;

    // Тут можно провести оптимизацию сравнив a и b и не обменивая их значениями в случае эквивалентности их значений.

    temp = a;
    a = b;
    b = temp;
    
    std::cout << a << ' ' << b << std::endl;
}
