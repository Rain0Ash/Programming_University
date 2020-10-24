#include <algorithm>
#include <iostream>
#include <string>
#include <vector>

class alcohol
{
public:
	std::string name;
	unsigned int price;
	unsigned int volume;
	double efficiency;

	bool operator == (const alcohol& value) const
	{
		return this->name == value.name && this->price == value.price && this->volume == value.volume && abs(this->efficiency - value.efficiency) < DBL_EPSILON;
	}

	bool operator != (const alcohol& value) const
	{
		return !(*this == value);
	}
	
	alcohol(const std::string name, const unsigned int price, const unsigned int volume)
	{
		this->name = name;
		this->price = price ? price : 1;
		this->volume = volume;
		this->efficiency = (double)this->volume / (double)this->price;
	}
};

class money
{
public:
	unsigned int value;


	money(const unsigned int value)
	{
		this->value = value;
	}
};

bool try_get_input_alcohol(std::string& name, int& price, int& volume)
{
	std::cin >> name >> price >> volume;
	if (std::cin.fail())
	{
		std::cin.clear();
		std::cin.ignore(32767, '\n');
		return false;
	}

	std::cin.ignore(32767, '\n');
	return true;
}

alcohol try_read_alcohol()
{
	std::string name;
	int price;
	int volume;
	while (!try_get_input_alcohol(name, price, volume))
	{
		std::cout << "Неверный ввод. Повторите попытку." << std::endl;
	}

	return alcohol(name, price, volume);
}

bool try_get_input(unsigned int& x)
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

void try_read(unsigned int& value)
{
	while (!try_get_input(value))
	{
		std::cout << "Неверный ввод. Повторите попытку." << std::endl;
	}
}

void get_alco_from_market(money& currency, std::vector<alcohol> market, std::vector<std::pair<alcohol, unsigned int>>& store, const int id = 0)
{
	market = std::vector<alcohol>(market);

	if (market.empty())
	{
		return;
	}

	alcohol current = *std::max_element(market.begin(), market.end(),
	                                          [](const alcohol& first, const alcohol& second)
	                                          {
		                                          return first.efficiency < second.efficiency;
	                                          });

	unsigned int count = currency.value / current.price;
	currency.value %= current.price;

	store.push_back({ current, count });

	market.erase(std::remove(market.begin(), market.end(), current), market.end());

	for (const alcohol& alco : market)
	{
		if (alco.price <= currency.value)
		{
			get_alco_from_market(currency, market, store, id + 1);
		}
	}
}

void main()
{
	setlocale(LC_ALL, "Russian");

	unsigned int current_money;
	unsigned int n;

	try_read(current_money);
	money currency(current_money);

	try_read(n);

	std::vector<alcohol> market;
	for (unsigned int i = 0; i < n; i++)
	{
		alcohol current = try_read_alcohol();

		if (current.price <= currency.value && current.efficiency > 0)
		{
			market.push_back(current);
		}
	}

	std::cout << std::endl;

	if (market.empty())
	{
		std::cout << "-1" << std::endl;
		return;
	}
	
	unsigned int total = 0;

	std::vector<std::pair<alcohol, unsigned int>> store;
	get_alco_from_market(currency, market, store);

	for (auto const& pair : store)
	{
		if (pair.second <= 0)
		{
			continue;
		}

		total += pair.first.volume * pair.second;
		std::cout << pair.first.name << " " << pair.second << std::endl;
	}

	std::cout << total << std::endl;
	std::cout << currency.value << std::endl;
}
