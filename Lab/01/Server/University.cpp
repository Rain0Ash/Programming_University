#include <ctime>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>

#include <cpp_httplib/httplib.h>
#include <nlohmann/json.hpp>

#include <common/directory.hpp>
#include <common/time.hpp>
#include <common/weather.hpp>
#include <common/template.hpp>
#include <common/utils.hpp>

using namespace httplib;

#define UTF8 "text/html;charset=utf-8"
#define UTF8JSON "application/json;charset=utf-8"

std::string current_dir;
std::string executable_dir;

void generate_widget_response(const Request& req, Response& res)
{
	nlohmann::json json;
	if (try_get_hourly_weather_json(json))
	{
		std::string result;

		const std::string path = current_dir + "\\" + "template.html";
		
		if (try_read_and_set_template(json, path, result) || try_read_and_set_template(json, executable_dir + "template.html", result))
		{
			res.set_content(result, UTF8);
			return;
		}

		res.set_content("Шаблон: \"" + path + "\" недоступен." , UTF8);
		res.status = 503;
		return;
	}

	res.set_content("Неизвестная ошибка!", UTF8);
	res.status = 503;
}

void generate_response(const Request& req, Response& res, const std::function<bool(nlohmann::json&)>& generator)
{
	nlohmann::json json;
	if (generator(json))
	{
		res.set_content(json.dump(), UTF8JSON);
		return;
	}

	res.set_content("Неизвестная ошибка!", UTF8);
	res.status = 503;
}

void generate_raw_response(const Request& req, Response& res)
{
	return generate_response(req, res, try_get_out_weather_json);
}

void generate_hourly_response(const Request& req, Response& res)
{
	return generate_response(req, res, try_get_hourly_weather_json);
}

void generate_full_response(const Request& req, Response& res)
{
	return generate_response(req, res, try_get_weather_json);
}

int main(int argc, const char** argv)
{
	setlocale(LC_ALL, "Russian");

	try_read_cache_file();

	current_dir = get_current_working_directory();
	executable_dir = get_executable_directory(argv);
	change_directory(executable_dir.c_str());
	
	Server svr;

	svr.Get("/", generate_widget_response);
	svr.Get("/raw", generate_raw_response);
	svr.Get("/hourly", generate_hourly_response);
	svr.Get("/full", generate_full_response);

	std::cout << "Start server... OK" << std::endl;
	
	svr.listen("localhost", 3000);

	return 0;
}
