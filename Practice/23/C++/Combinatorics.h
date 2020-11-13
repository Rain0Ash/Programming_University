#ifndef COMBINATORICS_H
#define COMBINATORICS_H

#include "Factorial.h"

unsigned long long int combinations(unsigned long long int k, unsigned long long int n)
{
	return factorial(n) / (factorial(k) * factorial(n - k));
}

#endif

