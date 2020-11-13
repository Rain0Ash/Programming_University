#include <iostream>
#include <iomanip>
#include "Factorial.h"
#include "Taylor.h"
#include "Combinatorics.h"

#define PI 3.14159265358979323846

void main()
{
	setlocale(LC_ALL, "Russian");

	std::cout << "n\tn!" << std::endl;

	for (unsigned int n = 1; n <= 10; n++)
	{
		std::cout << n << "\t" << factorial(n) << std::endl;
	}

	std::cout << std::endl << "x\tsin(x)" << std::endl;
	
	for (double x = 0; x <= PI / 4; x += PI / 180)
	{
		std::cout << x << "\t" << std::setprecision(4) << taylor(x, 5) << std::endl;
	}

	std::cout << std::endl << "k\tC(k, 10)" << std::endl;

	for (unsigned int k = 1; k <= 10; k++)
	{
		std::cout << k << "\t" << combinations(k, 10) << std::endl;
	}
}
