#pragma once

#include "direct.h"

#include <cstring>
#include <string>
#include <iostream>

inline std::string get_executable_directory(const char** argv)
{
	const std::string path = argv[0];
	const size_t path_directory_index = path.find_last_of('\\');
	return path.substr(0, path_directory_index + 1);
}

inline bool change_directory(const char* dir)
{
	return _chdir(dir) == 0;
}

inline std::string get_current_working_directory()
{
	const int buffer_size = 4096;
	char buf[buffer_size];
	memset(buf, 0, buffer_size);
	_getcwd(buf, buffer_size - 1);
	return buf;
}
