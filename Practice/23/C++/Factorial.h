#pragma once

unsigned long long factorial(unsigned int n)
{
	unsigned long long factorial = 1;

    for (int i = 1; i <= n; ++i)
    {
        factorial *= i;
    }

    return factorial;
}