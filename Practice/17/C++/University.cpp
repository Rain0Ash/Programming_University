#include <algorithm>
#include <iostream>
#include <sstream>
#include <vector>
#include <unordered_set>

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

const int e = 37;

void try_read(int& value)
{
    while (!try_get_input(value) || value > e - 1)
    {
        std::cout << (value > (e - 1) ? "Значение больше 36. Повторите ввод." : "Неверный ввод. Повторите попытку.") << std::endl;
    }
}

void main()
{
    setlocale(LC_ALL, "Russian");

    const int k = 12;
    
    const bool redblack[e] = 
    {         false,
        true, false, true,
        false, true, false,
        true, false, true,
        false, false, true,
        false, true, false,
        true, false, true,
        true, false, true,
        false, true, false,
        true, false, true,
        false, false, true,
        false, true, false,
        true, false, true
    };

    int iteration = 0;
	
    int values[e] = {};
	
    std::vector<int> buffer(k);
    std::fill(std::begin(buffer), std::end(buffer), INT32_MIN);
	
    int red = 0;
    int black = 0;
	
    int value;
	
	do
	{
        try_read(value);

		if (value < 0)
		{
            break;
		}

        ++values[value];
        buffer[iteration % k] = value;

        if (value != 0)
        {
            redblack[value] ? ++red : ++black;
        }

        const int max = *std::max_element(std::begin(values), std::end(values));
		
        std::stringstream stream;

        std::string result;

		for (int i = 0; i < e; i++)
		{
			if (values[i] == max)
			{
                stream << i << " ";
			}
		}

        result = stream.str();

        if (!result.empty())
        {
            result.resize(result.size() - 1);
        	stream.str(std::string());
        }

        std::cout << result << std::endl;
        
		for (int i = 0; i < e; i++)
		{
			if (!std::any_of(std::begin(buffer), std::end(buffer), [&](const int val) { return val == i; }))
			{	
                stream << i << " ";
			}
		}

        result = stream.str();

		if (!result.empty())
		{
            result.resize(result.size() - 1);
            stream.str(std::string());
		}
        
        std::cout << result << std::endl;

        std::cout << red << " " << black << std::endl << std::endl;
		
    } while (++iteration);
}
