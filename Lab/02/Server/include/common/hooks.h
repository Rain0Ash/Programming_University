#pragma once

#include <fstream>

#include "config.h"
#include "cpp_httplib/httplib.h"
#include "nlohmann/json.hpp"

using json = nlohmann::json;

const std::string page_name = "webhooks.html";
const std::string hook_template =
    R"(
    <div class="form-row align-items-center">
        <div class="col">
            <input type="text" value="{Webhook URL}" class="form-control mb-2" disabled>
        </div>
    
        <div class="col">
            <button type="submit" name="del" value="{Webhook URL}" class="btn btn-danger mb-2">Удалить</button>
        </div>
    </div>)";

json config;

std::string generate_hook_page()
{
    std::ifstream webpage_file(page_name);
    std::string webpage;
    
    if (!webpage_file.is_open())
    {
        log(u8"Файл шаблона не удалось открыть", message_type::error);
        return "";
    }

    std::getline(webpage_file, webpage, '\0');
    webpage_file.close();
    
    if (config.empty())
    {
        try_read_config(config);
    }
    
    std::string hooks;
    if (!config["webhooks"].empty())
    {
        for (const auto& hook : config["webhooks"])
        {
            if (!hook.is_string())
            {
                log(std::string("В файле конфигурации обнаружен неверный тип ") + hook.type_name(), message_type::error);
                continue;
            }

            std::string htemplate = hook_template;
            replace(htemplate, "{Webhook URL}", hook.get<std::string>());
            hooks += htemplate;
        }
    }

    replace(webpage, "{webhooks_list}", hooks);
    return webpage;
}

void hooks_get(const httplib::Request& req, httplib::Response& res)
{
    res.set_content(generate_hook_page(), "text/html; charset=UTF-8");
}

bool try_del_hook(const httplib::Request& req, httplib::Response& res)
{
    std::string value = req.get_param_value("del");

    if (value.empty())
    {
        log("Пустой запрос на удаление вебхука.", message_type::warning);
        return false;
    }
    
    log(std::string("В POST получен параметр del=") + value, message_type::info);

    if (config["webhooks"].empty())
    {
        log("Вебхуки отсутствуют.", message_type::warning);
        return true;
    }

    const size_t size = config["webhooks"].size();
    for (size_t i = 0; i < size; i++)
    {
	    std::string hook = config["webhooks"][i];
        if (hook == value)
        {
            config["webhooks"].erase(config["webhooks"].begin() + i);
            log(std::string("Был удален вебхук ") + hook, message_type::info);
        }
    }
	
    return true;
}

bool contains_hook(const std::string& value)
{
    if (config["webhooks"].empty())
    {
        return false;
    }

	for (const auto& item : config["webhooks"])
	{
        if (item.is_string() && item.get<std::string>() == value)
        {
            return true;
        }
	}

    return false;
}

bool try_set_hook(const httplib::Request& req, httplib::Response& res)
{
    std::string value = req.get_param_value("set");
    if (value.empty())
    {
        log("Пустой запрос на создание вебхука.", message_type::info);
        return false;
    }

    log(std::string("В POST получен параметр set=") + value, message_type::info);

    if (contains_hook(value))
    {
        log(std::string("Вебхук ") + value + std::string(" уже существует в конфигурации."), message_type::warning);
        return true;
    }

    config["webhooks"].push_back(value);
    log(std::string("Добавлен вебхук ") + value, message_type::info);
    return true;
}

void hooks_post(const httplib::Request& req, httplib::Response& res)
{
    if (config.empty())
    {
        try_read_config(config);
    }

	if (req.has_param("set"))
    {
        try_set_hook(req, res);
        try_save_config(config);
    }
    else if (req.has_param("del"))
    {
        try_del_hook(req, res);
        try_save_config(config);
    }

    res.set_content(generate_hook_page(), "text/html; charset=UTF-8");
}
