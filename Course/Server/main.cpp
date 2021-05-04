// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include <csignal>

#include "nlohmann/json.hpp"

#include "instance.hpp"
#include "server.hpp"
#include "database.hpp"

#include "config.hpp"

#include "directory.hpp"
#include "logger.hpp"
#include "responses.hpp"

#include "user.hpp"


int main(int argc, const char** argv)
{
    setlocale(LC_ALL, "russian");

    directory::change_directory(directory::get_executable_directory(argv).c_str());

    logger::init("log.txt");
	
    nlohmann::json config;

    if (!config::try_read_config(config))
    {
        config::try_save_config(config);
    }

    std::string host;
    std::string portcfg;
    std::string https;
	
    std::string certificate;
    std::string privatekey;
	
    std::string schema_name;
    std::string users_table_name;
    std::string fishing_table_name;
	
    std::string peper;
    

    config::try_read_property(config, "host", host, "localhost");
    config::try_read_property(config, "port", portcfg, "3000");

    short unsigned int port = 3000;
	try
	{
        auto value = std::stoul(portcfg);

		if (value <= UINT16_MAX)
		{
            port = (unsigned short int) value;
		}
	}
	catch (...)
	{
	}

    config::try_read_property(config, "certificate", certificate, "certificate.crt");
    config::try_read_property(config, "privatekey", privatekey, "private.key");

    if (!config::try_read_property(config, "https", https) || https != "true" && https != "false")
    {
    	if (!instance::init_server(certificate.c_str(), privatekey.c_str(), host, port))
    	{
            logger::log("Can't initialize server.", message_type::fatal);
            system("pause");
            return 2;
    	}
    }
	else if (https == "true")
	{
        if (!instance::init_ssl_server(certificate.c_str(), privatekey.c_str(), host, port))
        {
            logger::log("Can't initialize server. Maybe SSL certificate not found. Install certificate or disable HTTPS mode.", message_type::fatal);
            system("pause");
            return 3;
        }
	}
    else
    {
        if (!instance::init_server(host, port))
        {
            logger::log("Can't initialize server.", message_type::fatal);
            system("pause");
            return 2;
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

    auto ctrlc_handler = [](sig_atomic_t signal)
    {
        logger::log("Stop server");
        logger::log("Database disconnect");

        instance::destroy();

        logger::log("Database disconnected", message_type::good);
        logger::log("Server stopped", message_type::good);
    	
        logger::close();
        system("pause");
        exit(0);
    };
	
    signal(SIGINT, ctrlc_handler);

    if (!instance::init_database(schema_name, users_table_name, fishing_table_name))
    {
        logger::log("Can't initialize to database", message_type::fatal);
        system("pause");
        return 4;
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
            system("pause");
            return 4;
        }

        logger::log("Database connected", message_type::good);

        if (!instance::get_database().create_schema())
        {
            logger::log("Can't create schema", message_type::fatal);
            system("pause");
            return 5;
        }

        if (!instance::get_database().create_users_table())
        {
            logger::log("Can't create users table", message_type::fatal);
            system("pause");
            return 6;
        }

        if (!instance::get_database().create_fishing_table())
        {
            logger::log("Can't create fishing table", message_type::fatal);
            system("pause");
            return 7;
        }

        logger::log("Start server");
        instance::get_server().listen();
        logger::log("Server started", message_type::good);

        instance::get_server().block();
    }
    catch (SAException& exception)
    {
        database::log_sqlapi_exception(exception);
    }
	catch (std::exception& exception)
	{
        logger::log(std::string("Unknown error: ") + exception.what(), message_type::fatal);
	}

    logger::log("Stop server");
    logger::log("Database disconnect");
	
    instance::destroy();

    logger::log("Database disconnected", message_type::good);
    logger::log("Server stopped", message_type::good);
	
    logger::close();

    system("pause");
}
