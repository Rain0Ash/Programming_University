#pragma once
#include <string>

#include <cpp_httplib/httplib.h>
#include <nlohmann/json.hpp>

#include "utils.hpp"

static Client weather_api("http://api.openweathermap.org");

static const std::string request_str = "/data/2.5/onecall?lat=44.957191&lon=34.11079&exclude=minutely,daily&units=metric&lang=ru&appid=5ced5dc06c1e4991ac7b402ab6426e01";

bool try_get_weather_request(std::string& str)
{
	Result request = weather_api.Get(request_str.c_str());

	if (request)
	{
		if (request->status == 200 || request->status == 203)
		{
			str = request->body;
			return true;
		}

		std::cerr << "Weather status code: " << request->status << std::endl;
	}
	else
	{
		std::cerr << "Weather error code: " << request.error() << std::endl;
	}

	return false;
}

bool try_get_weather_json_request(nlohmann::json& json)
{
	std::string str;
	if (try_get_weather_request(str))
	{
		replace(str, "\\\"", "\"");
		json = nlohmann::json::parse(str);
		return true;
	}

	return false;
}

static nlohmann::json cache;

bool try_read_cache_file()
{
	try
	{
		std::ifstream cache_stream("cache.json");

		const bool successful = cache_stream.is_open() && !is_empty(cache_stream);
		if (successful)
		{
			cache_stream >> cache;
		}

		cache_stream.close();
		return successful;
	}
	catch (nlohmann::json::parse_error&)
	{
		return false;
	}
}

bool try_write_cache_file(const nlohmann::json& json)
{
	if (json.empty())
	{
		return false;
	}

	std::ofstream cache_file("cache.json");
	cache_file << cache;
	cache_file.close();
	return true;
}

bool try_get_weather_json_internal(nlohmann::json& json, const time_t& current_time)
{
	if (cache != nullptr && !cache.empty())
	{
		json = cache;
	}
	else
	{
		if (!try_get_weather_json_request(json))
		{
			return false;
		}

		cache = json;

		try_write_cache_file(json);
	}
	
	nlohmann::json hourly = json["hourly"];

	const int size = hourly.size();

	if (hourly[size - 1]["dt"].get<time_t>() < current_time)
	{
		cache = nullptr;
		return try_get_weather_json_internal(json, current_time);
	}
	
	return !json.empty();
}

bool try_get_weather_json(nlohmann::json& json)
{
	time_t current_time;
	try_get_current_unix_time(current_time);
	
	return try_get_weather_json_internal(json, current_time);
}

bool try_get_hourly_weather_json(nlohmann::json& json)
{
	time_t current_time;
	try_get_current_unix_time(current_time);
	
	if (!try_get_weather_json_internal(json, current_time))
	{
		return false;
	}

	nlohmann::json hourly = json["hourly"];
	
	const int size = hourly.size();
	
	for (int i = 0; i < size - 1; ++i)
	{
		if (hourly[i]["dt"].get<time_t>() >= current_time)
		{
			json = hourly[i];
			break;
		}
	}

	return !json.empty();
}

bool try_get_out_weather_json(nlohmann::json& json)
{
	if (!try_get_hourly_weather_json(json))
	{
		return false;
	}

	nlohmann::json out;
	out["temp"] = json["temp"];
	out["description"] = json["weather"][0]["description"];

	json = out;
	return !json.empty();
}

