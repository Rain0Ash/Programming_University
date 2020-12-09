#include <iostream>
#include <sstream>
#include <map>
#include <vector>

bool try_get_input(long long unsigned int& value)
{
    std::cin >> value;
    if (std::cin.fail())
    {
        std::cin.clear();
        std::cin.ignore(32767, '\n');
        return false;
    }

    std::cin.ignore(32767, '\n');
    return true;
}

void try_read(long long unsigned int& value)
{
    while (!try_get_input(value))
    {
        std::cout << "Неверный ввод. Повторите попытку." << std::endl;
    }
}

std::map<long long unsigned int, long long unsigned int> factors(long long unsigned int value)
{
    std::map<long long unsigned int, long long unsigned int> map;

    while (value > 1)
    {
	    for (long long unsigned int i = 2; i <= value; i++)
	    {
		    if (!(value % i))
		    {
                value /= i;

                map[i] = map.count(i) ? map[i] + 1 : 1;
				break;
		    }
	    }
    }
	
    return map;
}

std::string join(const std::vector<std::string>& elements, const char* const delimiter)
{
    std::ostringstream os;
    auto b = begin(elements);
    const std::vector<std::string>::const_iterator e = end(elements);

    if (b != e)
    {
        std::copy(b, prev(e), std::ostream_iterator<std::string>(os, delimiter));
        b = prev(e);
        os << *b;
    }

    return os.str();
}

int main()
{	
	setlocale(LC_ALL, "Russian");

    long long unsigned int value;
    try_read(value);

	if (value <= 3)
	{
        std::cout << value << std::endl;
        return 0;
	}

    std::vector<std::string> strings;

	for (const auto& pair : factors(value))
	{
		if (pair.second > 1)
		{
            strings.push_back(std::to_string(pair.first) + "^" + std::to_string(pair.second));
			continue;
		}

        strings.push_back(std::to_string(pair.first));
	}

    std::cout << join(strings, "*") << std::endl;
}