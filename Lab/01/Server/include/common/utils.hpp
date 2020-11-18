#pragma once
#include <string>

void replace(std::string& str, const std::string& from, const std::string& to)
{
	size_t position = str.find(from);
	while (position != std::string::npos)
	{
		str.replace(position, from.size(), to);
		position = str.find(from, position + to.size());
	}
}

bool is_empty(std::ifstream& stream)
{
	return stream.peek() == std::ifstream::traits_type::eof();
}