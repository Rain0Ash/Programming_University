// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "logger.hpp"

#include <ctime>
#include <fstream>
#include <iostream>
#include <sstream>

logger* logger::logger_ = nullptr;

logger::logger(const std::string& path, const bool append)
{
	this->file_ = new std::ofstream(path, append ? std::fstream::app : std::fstream::out);
}

logger::~logger()
{
	if (!this->file_)
	{
		return;
	}
	
	try
	{
		this->file_->close();
		delete this->file_;
		this->file_ = nullptr;
	}
	catch (...)
	{
	}
}

bool logger::good() const
{
	return this->file_ != nullptr && this->file_->good();
}

bool logger::msglog(const std::string& message) const
{
	if (!this->file_->is_open())
	{
		return false;
	}

	*this->file_ << message;
	return true;
}


bool logger::init(const std::string& path, const bool append)
{
	delete logger_;
	logger_ = new logger(path, append);
	return logger_->good();
}

bool logger::log(const std::string& message, const message_type& type)
{
	const time_t time = std::time(nullptr);
	std::string message_type;

	switch (type)
	{
	case message_type::info:
		message_type = "Info";
		break;
	case message_type::good:
		message_type = "Good";
		break;
	case message_type::warning:
		message_type = "Warning";
		break;
	case message_type::error:
		message_type = "Error";
		break;
	case message_type::critical:
		message_type = "Critical";
		break;
	case message_type::fatal:
		message_type = "Fatal";
		break;
	default:
		message_type = "Unknown";
		break;
	}

	char buffer[256];
	tm tm;
	localtime_s(&tm, &time);
	strftime(buffer, sizeof buffer, "%d.%m.%Y %H:%M:%S", &tm);

	const std::string str = "[" + message_type + "](" + std::string(buffer) + ") " + message + "\n";

	std::cout << str;
	return logger_ && logger_->msglog(str);
}

bool logger::close()
{
	delete logger_;
	return true;
}
