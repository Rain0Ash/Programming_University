#pragma once

#include <fstream>
#include <string>

enum class message_type
{
	info,
	good,
	warning,
	error,
	critical,
	fatal
};

class logger
{
private:
	static logger* logger_;
	std::ofstream* file_ = nullptr;
	
	logger(const std::string& path, bool append = false);
	~logger();

	bool good() const;
	bool msglog(const std::string& message) const;
public:
	logger() = delete;
	static bool init(const std::string& path, bool append = false);
	static bool log(const std::string& message, const message_type& type = message_type::info);
	static bool close();
};