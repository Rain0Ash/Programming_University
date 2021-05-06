// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "config.hpp"

#include "logger.hpp"
#include "utils.hpp"

bool config::try_read_property(const json& json, const std::string& property, std::string& result)
{
	try
	{
		result = json[property.c_str()].get<std::string>();
		return true;
	}
	catch (...)
	{
		return false;
	}
}

bool config::try_read_property(const json& json, const std::string& property, std::string& result, const std::string& alternate)
{
	if (!try_read_property(json, property, result))
	{
		result = alternate;
		return false;
	}

	return true;
}

bool config::try_read_config(json& config)
{
	config = json();

	std::ifstream config_file(filename);
	if (config_file.is_open() && !utils::is_empty(config_file))
	{
		logger::log("Start process of configuration file deserialization", message_type::info);

		try
		{
			config_file >> config;
		}
		catch (std::exception& ex)
		{
			logger::log(std::string("Configuration file deserialization error: ") + ex.what(), message_type::error);
			config_file.close();
			return false;
		}

		logger::log("Configuration file successful deserialized", message_type::info);
		config_file.close();

		return true;
	}

	if (config_file.good())
	{
		logger::log("Configuration file is exists, but can't be opened", message_type::error);
	}
	else
	{
		logger::log("Configuration file doesn't exists", message_type::warning);
	}

	config["host"] = "localhost";
	config["port"] = "3000";
	config["https"] = "default";
	config["certificate"] = "certificate.crt";
	config["privatekey"] = "private.key";
	config["database"] = "Fisher";
	config["dblogin"] = "sa";
	config["dbpassword"] = "SQL";
	config["schema_name"] = "Fisher";
	config["users_table_name"] = "Users";
	config["fishing_table_name"] = "Fishing";
	config["peper"] = "Peper";
	config["max_cheat_requests"] = "10";
	config["cheat_request_timeout"] = "1000";
	config["log_file_append"] = "false";

	return false;
}

bool config::try_save_config(const json& config)
{
	std::ofstream config_file(filename);

	if (config_file.is_open())
	{
		logger::log("Configuration file saving", message_type::info);

		try
		{
			config_file << config.dump(4);
		}
		catch (...)
		{
			config_file.close();
			return false;
		}

		config_file.close();
		return true;
	}

	logger::log("Configuration file creating failed.", message_type::error);
	return false;
}
