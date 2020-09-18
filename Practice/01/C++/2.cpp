#include <iostream>

void main()
{
    int ifirst = 3;
    int isecond = 3.14;
    double dfirst = 3;
    double dsecond = 3.14;

    std::cout << ifirst << std::endl << isecond << std::endl << dfirst << std::endl << dsecond << std::endl;
    // Данный результат объясняется тем, что тип integer может содержать только целое число. При попытке присвоить ему число типа с плавающей запятой, нецелочисленная часть будет откинута.
}
