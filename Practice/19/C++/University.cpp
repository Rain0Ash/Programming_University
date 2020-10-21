#include <iostream>
#include <string>
#include <vector>
#include <unordered_set>
#include <sstream>

bool try_get_input(int& x)
{
	std::cin >> x;
	if (std::cin.fail())
	{
		std::cin.clear();
		std::cin.ignore(32767, '\n');
		return false;
	}

	std::cin.ignore(32767, '\n');
	return true;
}

void try_read(int& value)
{
	while (!try_get_input(value))
	{
		std::cout << "Неверный ввод. Повторите попытку." << std::endl;
	}
}

void zfill(std::string& str, const size_t num)
{
	if (num > str.size()) str.insert(0, num - str.size(), '0');
}

std::string convert(int value, int n, int k)
{
	std::stringstream stream;

	while (value > 0)
	{
		stream << std::to_string(value % k);
		value /= k;
	}

	std::string result = stream.str();
	std::reverse(result.begin(), result.end());
	zfill(result, n);
	return result;
}


void main()
{
	setlocale(LC_ALL, "Russian");

	std::cout << "Введите число." << std::endl;
	int n;
	std::string k;

	while (true)
	{
		try_read(n);

		if (n < 1 || n > 8)
		{
			std::cout << "Число n находится вне границ [1; 8]. Повторите ввод." << std::endl;
			continue;
		}

		std::getline(std::cin, k);

		if (k.length() < 1 || k.length() > n)
		{
			std::cout << "Длина строки меньше 1 или больше n. Повторите ввод." << std::endl;
			continue;
		}

		break;
	}

	std::vector<std::string> converted;

	int klen = k.length();
	for (int i = 0; i < (int)std::pow(klen, n); i++)
	{
		converted.push_back(convert(i, n, klen));
	}

	std::unordered_set<char> symbols;
	for (int i = 0; i < klen; i++)
	{
		symbols.insert(i + '0');
	}

	std::vector<std::string> validated;

	for (const std::string& word : converted)
	{
		std::unordered_set<char> set;

		for (const char& chr : word)
		{
			set.insert(chr);
		}

		if (set == symbols)
		{
			validated.push_back(word);
		}
	}

	std::stringstream result;

	for (int i = 0; i < validated.size(); i++)
	{
		for (const char& chr : validated[i])
		{
			result << k[chr - '0'];
		}

		if (i < validated.size() - 1)
		{
			result << " ";
		}
	}

	std::cout << result.str() << std::endl;
}
