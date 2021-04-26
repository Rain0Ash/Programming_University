#pragma once

#include <fstream>
#include <string>

inline void replace(std::string& str, const std::string& from, const std::string& to)
{
	size_t position = str.find(from);
	while (position != std::string::npos)
	{
		str.replace(position, from.size(), to);
		position = str.find(from, position + to.size());
	}
}

inline bool is_empty(std::ifstream& stream)
{
	return stream.peek() == std::ifstream::traits_type::eof();
}

inline void trim_start(std::string& s)
{
	s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](const unsigned char& ch)
	{
		return !std::isspace(ch);
	}));
}

inline void trim_end(std::string& s)
{
	s.erase(std::find_if(s.rbegin(), s.rend(), [](const unsigned char& ch)
	{
		return !std::isspace(ch);
	}).base(), s.end());
}

inline void trim(std::string& s)
{
	trim_start(s);
	trim_end(s);
}

inline std::string trim_copy_start(std::string s)
{
	trim_start(s);
	return s;
}

inline std::string trim_copy_end(std::string s)
{
	trim_end(s);
	return s;
}

inline std::string trim_copy(std::string s)
{
	trim(s);
	return s;
}

inline uint64_t time_epoch()
{
	return time(nullptr) * 1000;
}
