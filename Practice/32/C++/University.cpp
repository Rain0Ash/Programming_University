#include <sstream>
#include <vector>
#include <array>
#include <string>
#include <fstream>
#include <algorithm>

struct Passenger
{
	std::string name;
	std::string cabin;
	std::string ticket;
	float fare;
	int id;
	bool survival;
	uint8_t age;
	uint8_t pclass;
	uint8_t sex;
	uint8_t sublings;
	uint8_t parents;
	uint8_t embarked;
};

struct Passenger_str
{
	std::string name;
	std::string cabin;
	std::string ticket;
	std::string fare;
	std::string id;
	std::string survival;
	std::string pclass;
	std::string sex;
	std::string age;
	std::string sublings;
	std::string parents;
	std::string embarked;
};

std::vector<std::vector<std::string>> parse_cvs(std::istream& out)
{
	std::string line;
	std::string buffer;

	std::vector<std::string> separated;
	std::vector<std::vector<std::string>> result;

	char depth = 0;
	char position = 0;

	while (std::getline(out, line, '\r'))
	{
		position = 0;
		depth = 0;

		for (char chr : line)
		{
			if (chr == ',')
			{
				switch (depth)
				{
					case 0:
						separated.push_back(buffer);
						buffer.clear();
						++position;
						continue;
					case 2:
						separated.push_back(buffer);
						buffer.clear();
						depth = 0;
						++position;
						continue;
					default:
						break;
				}
			}
			
			if (chr == '"')
			{
				switch (depth)
				{
					case 0:
						depth = 1;
						continue;
					case 1:
						depth = 2;
						continue;
					case 2:
						buffer.push_back('"');
						depth = 1;
						continue;
					default:
						break;
				}
			}
			
			buffer.push_back(chr);
		}
		
		separated.push_back(buffer);
		buffer.clear();

		result.push_back(separated);
		separated.clear();
	}
	
	return result;
}

std::string replace(std::string str, const std::string& from, const std::string& to)
{
	size_t start_pos = 0;
	while ((start_pos = str.find(from, start_pos)) != std::string::npos)
	{
		str.replace(start_pos, from.length(), to);
		start_pos += to.length();
	}
	
	return str;
}

std::istream& operator >>(std::istream& stream, std::vector<Passenger>& vec)
{
	auto matrix = parse_cvs(stream);

	for (auto row : matrix)
	{
		Passenger p;

		p.id = std::stoi(row[0]);
		p.survival = std::stoi(row[1]);
		p.pclass = std::stoi(row[2]);
		p.name = row[3];
		p.sex = row[4] == "male" ? 'm' : 'f';
		p.age = row[5].empty() ? 0 : std::stof(row[5]);
		p.sublings = std::stoi(row[6]);
		p.parents = std::stoi(row[7]);
		p.ticket = row[8];
		p.fare = std::stof(row[9]);
		p.cabin = row[10];
		p.embarked = row[11][0];

		vec.push_back(p);
	}

	return stream;
}

std::ostream& operator <<(std::ostream& stream, std::vector<Passenger>& vector)
{
	const char separator = ',';
	const char end = '\r';
	for (auto& pass : vector)
	{
		std::string name = pass.name;
		name = replace(name, "\"", "\"\"");

		stream << pass.id << separator
			<< pass.survival << separator
			<< static_cast<int>(pass.pclass) << separator
			<< '"' << name << '"' << separator
			<< (pass.sex == 'm' ? "male" : "female") << separator
			<< static_cast<int>(pass.age) << separator
			<< static_cast<int>(pass.sublings) << separator
			<< static_cast<int>(pass.parents) << separator
			<< pass.ticket << separator
			<< pass.fare << separator
			<< pass.cabin << separator
			<< pass.embarked
			<< end << std::flush;
	}
	return stream;
}

void sort(std::vector<Passenger*>& vector)
{
	for (size_t i = 0; i < vector.size() - 1; ++i)
	{
		for (size_t j = vector.size() - 1; j > i; --j)
		{
			if (vector[j]->age < vector[j - 1]->age)
			{
				std::swap(*vector[j], *vector[j - 1]);
			}
		}
	}
}

int main()
{
	std::ifstream file("train.csv");
	std::ofstream output("out.csv");
	std::vector<Passenger> passengers;

	file >> passengers;
	std::vector<Passenger*> finded;

	for (auto& passenger : passengers)
	{
		if (passenger.pclass == 1 && passenger.sex == 'f' && passenger.survival)
		{
			finded.push_back(&passenger);
		}
	}

	sort(finded);

	output << passengers;
	
	file.close();
	output.close();
}
