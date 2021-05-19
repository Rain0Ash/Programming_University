// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include <csignal>
#include <conio.h>

#include "anticheat.hpp"
#include "nlohmann/json.hpp"

#include "instance.hpp"
#include "server.hpp"
#include "database.hpp"

#include "config.hpp"

#include "directory.hpp"
#include "logger.hpp"
#include "responses.hpp"

#include "user.hpp"

void internal_exit_handler(sig_atomic_t signal = 0)
{
	volatile static bool stopping = false;
	volatile static bool stopped = false;
	
	while (stopping)
	{
		Sleep(1);
	}

	if (stopped)
	{
		return;
	}

	stopping = true;

	bool server = false;
	bool database = false;

	try
	{
		instance::get_server();
		server = true;
	}
	catch (std::exception&)
	{
	}

	try
	{
		instance::get_database();
		database = true;
	}
	catch (std::exception&)
	{
	}

	if (server)
	{
		logger::log("Stop server");
	}

	if (database)
	{
		logger::log("Database disconnect");
	}

	instance::destroy();

	if (database)
	{
		logger::log("Database disconnected", message_type::good);
	}

	if (server)
	{
		logger::log("Server stopped", message_type::good);
	}

	switch (signal)
	{
	case 0:
	case 21:
		logger::log("Successful exit", message_type::good);
		break;
	case 2:
		logger::log("Successful ctrl-c break exit", message_type::good);
		signal = 0;
		break;
	default:
		std::string s_code = std::string("Exit code: {0}");
		utils::replace(s_code, "{0}", std::to_string(signal));
		logger::log(s_code, message_type::warning);
		break;
	}

	logger::close();

	if (signal != 21)
	{
		auto _ = _getch();
	}
	
	stopped = true;
	stopping = false;

	exit(signal);
};

int exit_handler(const int code = 0)
{
	internal_exit_handler(code);
	return code;
}

int main(int argc, const char** argv)
{
	signal(SIGINT, internal_exit_handler);
	signal(SIGBREAK, internal_exit_handler);
	
	setlocale(LC_ALL, "russian");

	directory::change_directory(directory::get_executable_directory(argv).c_str());
	
	nlohmann::json config;

	if (!config::try_read_config(config))
	{
		config::try_save_config(config);

		logger::log("Please, edit configuration file and press any key:", message_type::warning);
		auto _ = _getch();
		
		if (!config::try_read_config(config))
		{
			return exit_handler(1);
		}
	}

	std::string append_log_cfg;
	config::try_read_property(config, "log_file_append", append_log_cfg, "false");

	logger::init("log.txt", append_log_cfg == "true");
	
	std::string host;
	std::string port_cfg;
	std::string max_cheat_requests_cfg;
	std::string cheat_request_timeout_cfg;
	std::string https;
	
	std::string certificate;
	std::string privatekey;
	
	std::string schema_name;
	std::string users_table_name;
	std::string fishing_table_name;
	
	std::string peper;

	config::try_read_property(config, "host", host, "localhost");
	config::try_read_property(config, "port", port_cfg, "3000");

	short unsigned int port = 3000;
	try
	{
		auto value = std::stoul(port_cfg);

		if (value <= UINT16_MAX)
		{
			port = (unsigned short int) value;
		}
	}
	catch (...)
	{
	}

	config::try_read_property(config, "max_cheat_requests", max_cheat_requests_cfg, "10");

	unsigned int max_cheat_requests = 10;
	try
	{
		auto value = std::stoul(max_cheat_requests_cfg);

		if (value <= UINT32_MAX)
		{
			max_cheat_requests = (unsigned int) value;
		}
	}
	catch (...)
	{
	}

	config::try_read_property(config, "cheat_request_timeout", cheat_request_timeout_cfg, "1000");
	
	unsigned int cheat_request_timeout = 1000;
	try
	{
		auto value = std::stoul(cheat_request_timeout_cfg);

		if (value <= UINT32_MAX)
		{
			cheat_request_timeout = (unsigned int) value;
		}
	}
	catch (...)
	{
	}

	anticheat::set_max_cheat_requests(max_cheat_requests);
	anticheat::set_cheat_request_timeout(cheat_request_timeout);
	
	config::try_read_property(config, "certificate", certificate, "certificate.crt");
	config::try_read_property(config, "privatekey", privatekey, "private.key");

	if (!config::try_read_property(config, "https", https) || https != "true" && https != "false")
	{
		if (!instance::init_server(certificate.c_str(), privatekey.c_str(), host, port))
		{
			logger::log("Can't initialize server.", message_type::fatal);
			return exit_handler(3);
		}
	}
	else if (https == "true")
	{
		if (!instance::init_ssl_server(certificate.c_str(), privatekey.c_str(), host, port))
		{
			logger::log("Can't initialize server. Maybe SSL certificate not found. Install certificate and then press any key or disable HTTPS mode and restart server.", message_type::warning);
			auto _ = _getch();

			if (!instance::init_ssl_server(certificate.c_str(), privatekey.c_str(), host, port))
			{
				logger::log("Can't initialize server.", message_type::fatal);
				return exit_handler(3);
			}
		}
	}
	else
	{
		if (!instance::init_server(host, port))
		{
			logger::log("Can't initialize server.", message_type::fatal);
			return exit_handler(3);
		}
	}
	
	instance::get_server().get("/", responses::generate_index_response);
	instance::get_server().get("/register", responses::generate_register_response);
	instance::get_server().get("/statistics", responses::generate_statistics_response);
	instance::get_server().get("/fishing", responses::generate_fishing_response);
	instance::get_server().get("/records", responses::generate_records_response);
	instance::get_server().get("/close", responses::generate_close_response);

	config::try_read_property(config, "schema_name", schema_name, "Fisher");

	config::try_read_property(config, "users_table_name", users_table_name, "Users");

	config::try_read_property(config, "fishing_table_name", fishing_table_name, "Fishing");

	config::try_read_property(config, "peper", peper, "Peper");
	user::set_peper(peper);

	if (!instance::init_database(schema_name, users_table_name, fishing_table_name))
	{
		logger::log("Can't initialize to database", message_type::fatal);
		return exit_handler(4);
	}
	
	try
	{
		std::string dbname;
		config::try_read_property(config, "database", dbname, "Fisher");
		std::string dblogin;
		config::try_read_property(config, "dblogin", dblogin, "sa");
		std::string dbpassword;
		config::try_read_property(config, "dbpassword", dbpassword, "SQL");

		logger::log("Start database connection");
		
		if (!instance::get_database().connect_to_database(dbname.c_str(), dblogin.c_str(), dbpassword.c_str()))
		{
			logger::log("Can't connect to database", message_type::fatal);
			return exit_handler(4);
		}

		logger::log("Database connected", message_type::good);

		if (!instance::get_database().create_schema())
		{
			logger::log("Can't create schema", message_type::fatal);
			return exit_handler(5);
		}

		if (!instance::get_database().create_users_table())
		{
			logger::log("Can't create users table", message_type::fatal);
			return exit_handler(6);
		}

		if (!instance::get_database().create_fishing_table())
		{
			logger::log("Can't create fishing table", message_type::fatal);
			return exit_handler(7);
		}

		if (!instance::get_database().register_stored_procedure())
		{
			logger::log("Can't register stored procedures", message_type::fatal);
			return exit_handler(8);
		}

		logger::log("Start server");
		instance::get_server().listen();
		logger::log("Server started", message_type::good);

		instance::get_server().block();
		return exit_handler(0);
	}
	catch (SAException& exception)
	{
		database::log_sqlapi_exception(exception);
	}
	catch (std::exception& exception)
	{
		logger::log(std::string("Unknown error: ") + exception.what(), message_type::fatal);
	}

	return exit_handler(INT32_MAX);
}
