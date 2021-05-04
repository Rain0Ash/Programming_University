#pragma once

#include <fstream>
#include <string>

class utils
{
public:
	static void replace(std::string& string, const std::string& from, const std::string& to);
	static bool is_empty(std::ifstream& stream);
	static void trim_start(std::string& string);
	static void trim_end(std::string& string);
	static void trim(std::string& string);
	static std::string trim_copy_start(std::string string);
	static std::string trim_copy_end(std::string string);
	static std::string trim_copy(std::string string);
	static uint64_t time_epoch();
};
