#pragma once

#include <fstream>
#include <string>

#include "utils.hpp"
#include "nlohmann/json.hpp"

using json = nlohmann::json;

const std::string cfg_name = "config.json";

bool try_read_config(json& config)
{
    config = json();
    
    std::ifstream config_file(cfg_name);
    if (config_file.is_open() && !is_empty(config_file))
    {
        log(u8"Идет десериализация файла конфигурации.", message_type::info);

        try
        {
            config_file >> config;
        }
        catch (std::exception& ex)
        {
            log(std::string(u8"Ошибка десериализации файла конфигурации ") + ex.what(), message_type::error);
            config_file.close();
            return false;
        }

        log(u8"Десериализация файла кофигурации успешно завершена.", message_type::info);
        config_file.close();

        return true;
    }
    
    if (config_file.good())
    {
        log(u8"Файл конфигурации существует, но его не удалось открыть.", message_type::error);
    }
    else
    {
        log(u8"Файл конфигурации не существует", message_type::warning);
    }
    
    config["webhooks"] = json::array();
    
    return false;
}

bool try_save_config(const json& config)
{
    std::ofstream config_file(cfg_name);

    if (config_file.is_open())
    {
        log(u8"Запись файла конфигурации", message_type::info);
        
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
    
    log(u8"Не удалось создать файл конфигурации.", message_type::error);
    return false;
}
