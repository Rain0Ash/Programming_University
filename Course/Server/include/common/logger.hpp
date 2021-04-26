#pragma once

#include <fstream>
#include <ctime>
#include <iostream>

static std::ofstream logger("log.txt");

enum class message_type
{
    info,
    warning,
    error,
    critical,
    fatal
};

bool log(const std::string& message, const message_type& type = message_type::info)
{
    const std::time_t time = std::time(nullptr);
    std::string message_type;
    
    switch (type)
    {
        case message_type::info:
            message_type = "Info";
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
    strftime(buffer, sizeof(buffer), "%d.%m.%Y %H:%M:%S", &tm);

    std::cout << "[" << message_type << "](" << std::string(buffer) << ") " << message << std::endl;
	
    if (!logger.is_open())
    {
        return false;
    }
	
    logger << "[" << message_type <<  "](" << std::string(buffer) << ") " << message << std::endl;
    return true;
}
