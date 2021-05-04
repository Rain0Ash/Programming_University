#pragma once

#include <string>

class directory
{
public:
	static std::string get_executable_directory(const char** argv);
	static bool change_directory(const char* dir);
	static std::string get_current_working_directory();
};
