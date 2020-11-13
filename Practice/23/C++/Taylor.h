#ifndef TAYLOR_H
#define TAYLOR_H

#include <cmath>
#include "Factorial.h"

double taylor(double x, unsigned int k)
{
	double result = 0;

	double negative = -1;
	
	for (unsigned int n = 0; n < k; n++)
	{
		negative *= -1;
		result += negative * std::pow(x, 2 * n + 1) / factorial(2 * n + 1);
	}

	return result;
}

#endif
