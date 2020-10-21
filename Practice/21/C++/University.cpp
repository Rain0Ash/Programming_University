#include <iostream>
#include <string>
#include <vector>
#include <unordered_set>
#include <sstream>

bool try_get_input(long double& weight, long double& height)
{
	std::cin >> weight >> height;
	if (std::cin.fail())
	{
		std::cin.clear();
		std::cin.ignore(32767, '\n');
		return false;
	}

	std::cin.ignore(32767, '\n');
	return true;
}

void try_read(long double& weight, long double& height)
{
	while (!try_get_input(weight, height))
	{
		std::cout << "Неверный ввод. Повторите попытку." << std::endl;
	}
}

double BMI(double weight, double height)
{
	return weight / (height * height);
}

void printBMI(double BMI)
{
	if (BMI < 18.5)
	{
		std::cout << "Underweight" << std::endl;
	}
	else if (BMI < 25)
	{
		std::cout << "Normal" << std::endl;
	}
	else if (BMI < 30)
	{
		std::cout << "Overweight" << std::endl;
	}
	else
	{
		std::cout << "Obesity" << std::endl;
	}
}

void main()
{
	setlocale(LC_ALL, "Russian");

	std::cout << "Введите числа." << std::endl;
	
	long double weight, height;
	
	while (true)
	{
		try_read(weight, height);
		
		if (weight < 0 || height < 0)
		{
			std::cout << "Одно или несколько чисел меньше нуля." << std::endl;
			continue;
		}

		break;
	}

	printBMI(BMI(weight, height / 100));
}
