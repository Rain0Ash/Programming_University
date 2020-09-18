#include <iostream>

void main()
{
    int temp;
    int a;
    int b;

    std::cin >> a >> b;

    temp = a;
    a = b;
    b = temp;
    
    std::cout << a << ' ' << b << std::endl;
}
