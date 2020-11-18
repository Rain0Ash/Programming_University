#pragma once
#include <cpp_httplib/httplib.h>
#include <nlohmann/json.hpp>

using namespace httplib;

static httplib::Client time_zone_api("http://worldtimeapi.org");

bool try_get_time(std::string& str)
{
	httplib::Result request = time_zone_api.Get("/api/timezone/Europe/Simferopol");

	if (request)
	{
		if (request->status == 200 || request->status == 203)
		{
			str = request->body;
			return true;
		}

		std::cerr << "Time status code: " << request->status << std::endl;
	}
	else
	{
		std::cerr << "Time error code: " << request.error() << std::endl;
	}

	return false;
}

static int unix_time = -1;

bool try_get_time_json(nlohmann::json& json)
{
	std::string str;
	if (try_get_time(str))
	{
		json = nlohmann::json::parse(str);
		return true;
	}

	return false;
}

bool try_get_current_unix_time(time_t& current)
{
	nlohmann::json json;
	if (!try_get_time_json(json))
	{
		current = std::time(nullptr);
		return false;
	}

	current = json["unixtime"].get<time_t>();
	return true;
}