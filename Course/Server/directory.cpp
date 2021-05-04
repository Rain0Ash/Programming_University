// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "directory.hpp"

#include <cstring>
#include <direct.h>
#include <string>

std::string directory::get_executable_directory(const char** argv)
{
	const std::string path = argv[0];
	const size_t path_directory_index = path.find_last_of('\\');
	
	return path.substr(0, path_directory_index + 1);
}

bool directory::change_directory(const char* dir)
{
	return _chdir(dir) == 0;
}

std::string directory::get_current_working_directory()
{
	const int size = 4096;
	char buffer[size];
	memset(buffer, 0, size);
	auto _ = _getcwd(buffer, size - 1);
	
	return buffer;
}