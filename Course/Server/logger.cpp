// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "logger.hpp"

#include <ctime>
#include <fstream>
#include <iostream>

logger* logger::logger_ = nullptr;

logger::logger(const std::string& path)
{
	this->file_ = new std::ofstream(path);
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

bool logger::msglog(const std::string& message, const message_type& type) const
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

	std::cout << "[" << message_type << "](" << std::string(buffer) << ") " << message << std::endl;

	if (!this->file_->is_open())
	{
		return false;
	}

	*this->file_ << "[" << message_type << "](" << std::string(buffer) << ") " << message << std::endl;
	return true;
}


bool logger::init(const std::string& path)
{
	delete logger_;
	logger_ = new logger(path);
	return logger_->good();
}

bool logger::log(const std::string& message, const message_type& type)
{
	return logger_ && logger_->msglog(message, type);
}

bool logger::close()
{
	delete logger_;
	return true;
}
