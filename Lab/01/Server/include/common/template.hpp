#pragma once
#include <fstream>
#include <iostream>
#include <ostream>
#include <string>
#include <common/utils.hpp>
#include <nlohmann/json.hpp>

bool try_read_template(const std::string& path, std::string& temp)
{
	std::ifstream file(path.c_str());
	if (file.is_open() && !is_empty(file))
	{
		std::getline(file, temp, '\0');
		file.close();
		return true;
	}

	file.close();
	std::cerr << "Error with opening template file: " << path << std::endl;
	return false;
}

static std::string _template;

bool try_reload_template(std::string& temp)
{
	if (try_read_template("template.html", temp))
	{
		_template = temp;
		return true;
	}

	return false;
}

bool try_read_template(std::string& temp)
{
	if (!_template.empty())
	{
		temp = _template;
		return true;
	}
	
	return try_reload_template(temp);
}

bool try_set_template(const nlohmann::json& json, const std::string& temp, std::string& result)
{
	try
	{
		result = temp;
		
		replace(result, "{hourly[i].weather[0].description}",
			json["weather"][0]["description"]);

		replace(result, "{hourly[i].weather[0].icon}", json["weather"][0]["icon"]);

		replace(result, "{hourly[i].temp}",
			std::to_string(static_cast<int>(std::round(json["temp"].get<double>()))));

		return true;
	}
	catch (const std::exception& _)
	{
		return false;
	}
}

bool try_set_template(const nlohmann::json& json, std::string& result)
{
	std::string temp;
	return try_read_template(temp) && try_set_template(json, temp, result);
}

bool try_read_and_set_template(const nlohmann::json& json, const std::string& path, std::string& result)
{
	std::string temp;
	return try_read_template(path, temp) && try_set_template(json, temp, result);
}