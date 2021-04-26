#pragma once

#include <fstream>
#include <string>

#include "logger.hpp"
#include "utils.hpp"
#include "nlohmann/json.hpp"

using json = nlohmann::json;

constexpr const char* cfg_name = "config.json";

template<typename T>
bool try_read_property(const json& json, const std::string& property, T& result)
{
    try
    {
        result = json[property.c_str()].get<T>();
        return true;
    }
    catch (std::exception exception)
    {
        return false;
    }
}

template<typename T>
bool try_read_property(const json& json, const std::string& property, T& result, const T& alternate)
{
    if (!try_read_property(json, property, result))
    {
        result = alternate;
        return false;
    }

    return true;
}

bool try_read_config(json& config)
{
    config = json();
    
    std::ifstream config_file(cfg_name);
    if (config_file.is_open() && !is_empty(config_file))
    {
        log("Start process of configuration file deserialization", message_type::info);

        try
        {
            config_file >> config;
        }
        catch (std::exception& ex)
        {
            log(std::string("Configuration file deserialization error: ") + ex.what(), message_type::error);
            config_file.close();
            return false;
        }

        log("Configuration file successful deserialized", message_type::info);
        config_file.close();

        return true;
    }
    
    if (config_file.good())
    {
        log("Configuration file is exists, but can't be opened", message_type::error);
    }
    else
    {
        log("Configuration file doesn't exists", message_type::warning);
    }

    config["host"] = "localhost";
    config["port"] = 3000;
    config["database"] = "Fisher";
    config["dblogin"] = "sa";
    config["dbpassword"] = "SQL";
    config["schema_name"] = "Fisher";
    config["users_table_name"] = "Users";
    config["fishing_table_name"] = "Fishing";
    config["peper"] = "Peper";
    config["patched"] = "true";
    
    return false;
}

bool try_save_config(const json& config)
{
    std::ofstream config_file(cfg_name);

    if (config_file.is_open())
    {
        log("Configuration file saving", message_type::info);
        
        try
        {
            config_file << config.dump(4);
        }
        catch (std::exception& _)
        {
            config_file.close();
            return false;
        }
        
        config_file.close();
        return true;
    }
    
    log("Configuration file creating failed.", message_type::error);
    return false;
}
