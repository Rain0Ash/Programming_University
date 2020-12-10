#include <iostream>
#include <sstream>
#include <map>
#include <vector>

bool try_get_input(unsigned int& value)
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

void try_read(unsigned int& value)
{
    while (!try_get_input(value))
    {
        std::cout << "Неверный ввод. Повторите попытку." << std::endl;
    }
}

std::map<unsigned int, unsigned int> factors(unsigned int value)
{
    std::map<unsigned int, unsigned int> map;

    while (value > 1)
    {
        for (unsigned int i = 2; i <= value; i++)
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

std::string join(const std::vector<std::string>& elements, const std::string& delimiter)
{
    std::ostringstream os;
    auto b = begin(elements);
    const std::vector<std::string>::const_iterator e = end(elements);

    if (b != e)
    {
        std::copy(b, prev(e), std::ostream_iterator<std::string>(os, delimiter.c_str()));
        b = prev(e);
        os << *b;
    }

    return os.str();
}

void print_factorization(unsigned int n)
{
    if (n <= 3)
    {
        std::cout << n << std::endl;
        return;
    }

    std::vector<std::string> strings;

    for (const auto& pair : factors(n))
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

int main()
{    
    setlocale(LC_ALL, "Russian");

    unsigned int value;
    try_read(value);

    print_factorization(value);
}