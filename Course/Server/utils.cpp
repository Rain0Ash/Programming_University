// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "utils.hpp"

#include <ctime>
#include <string>
#include <iostream>

void utils::replace(std::string& string, const std::string& from, const std::string& to)
{
	size_t position = string.find(from);
	while (position != std::string::npos)
	{
		string.replace(position, from.size(), to);
		position = string.find(from, position + to.size());
	}
}

bool utils::is_empty(std::ifstream& stream)
{
	return stream.peek() == std::ifstream::traits_type::eof();
}

void utils::trim_start(std::string& string)
{
	string.erase(string.begin(), std::find_if(string.begin(), string.end(), [](const unsigned char& ch)
		{
			return !std::isspace(ch);
		}));
}

void utils::trim_end(std::string& string)
{
	string.erase(std::find_if(string.rbegin(), string.rend(), [](const unsigned char& ch)
		{
			return !std::isspace(ch);
		}).base(), string.end());
}

void utils::trim(std::string& string)
{
	trim_start(string);
	trim_end(string);
}

std::string utils::trim_copy_start(std::string string)
{
	trim_start(string);
	return string;
}

std::string utils::trim_copy_end(std::string string)
{
	trim_end(string);
	return string;
}

std::string utils::trim_copy(std::string string)
{
	trim(string);
	return string;
}

uint64_t utils::time_epoch()
{
	return time(nullptr) * 1000;
}