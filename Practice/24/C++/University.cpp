#include <iostream>
#include <fstream>
#include <iomanip>
#include <string>
#include <map>
#include "directory.hpp"
#include "json.hpp"

#define IN_FILE "in.json"
#define OUT_FILE "out.json"


bool is_empty(std::ifstream& stream)
{
	return stream.peek() == std::ifstream::traits_type::eof();
}

int main(int argc, const char** argv)
{
	setlocale(LC_ALL, "Russian");

	std::string executable = get_executable_directory(argv);
	change_directory(executable.c_str());
	
	std::ifstream input(IN_FILE);
	if (!input.is_open() || is_empty(input))
	{
		std::cout << "Проверьте расположение файла" << " \"" << IN_FILE << "\". " << "Он должен находиться в папке с исполняемым файлом и не быть пустым." << std::endl;
		return 0;
	}

	nlohmann::json json;
	input >> json;

	input.close();

	std::map<size_t, size_t> users_map;
	for (auto& task : json)
	{
		if (!task["completed"].get<bool>())
		{
			continue;
		}

		const auto user_id = task["userId"].get<size_t>();

		if (users_map.find(user_id) == users_map.end())
		{
			users_map.insert({ user_id, static_cast<size_t>(1) });
			continue;
		}

		++users_map[user_id];
	}

	nlohmann::json users;

	for (const auto& pair : users_map)
	{
		users.push_back(
		{
				{"task_completed", pair.second},
				{"userId", pair.first}
			}
		);
	}

	std::ofstream output(OUT_FILE);
	output << std::setw(4) << users << std::endl;
	output.close();

	std::cout << "Успешно." << std::endl;
}
