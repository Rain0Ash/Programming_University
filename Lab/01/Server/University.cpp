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


void generate_response(const Request& req, Response& res)
{
	nlohmann::json json;
	if (try_get_weather_json(json))
	{
		std::string result;
		if (try_set_template(json, result))
		{
			res.set_content(result, "text/html");
			return;
		}
	}

	res.set_content("Error", "text/html");
	res.status = 503;
}


void generate_raw_response(const Request& req, Response& res)
{
	nlohmann::json json;
	if (try_get_out_weather_json(json))
	{
		res.set_content(json.dump(), "application/json");
		return;
	}

	res.set_content("Error", "text/html");
	res.status = 503;
}

int main(int argc, const char** argv)
{
	setlocale(LC_ALL, "Russian");

	try_read_cache_file();
	
	Server svr;

	svr.Get("/", generate_response);
	svr.Get("/raw", generate_raw_response);


	std::cout << "Start server... OK" << std::endl;
	
	svr.listen("localhost", 3000);
}
