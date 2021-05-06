#pragma once

#include <string>

#include "nlohmann/json.hpp"

using json = nlohmann::json;

class config
{
public:
	static constexpr const char* filename = "config.json";
	static bool try_read_property(const json& json, const std::string& property, std::string& result);
	static bool try_read_property(const json& json, const std::string& property, std::string& result, const std::string& alternate);
	static bool try_read_config(json& config);
	static bool try_save_config(const json& config);
};