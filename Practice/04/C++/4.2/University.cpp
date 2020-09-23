#include <iostream>

void main()
{
    int a;
    int b;

    std::cin >> a >> b;

    b = a + b;
    a = b - a;
    b = b - a;
    
    std::cout << a << ' ' << b << std::endl;
}
