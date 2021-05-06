#pragma once

#include "server.hpp"
#include "database.hpp"


class instance
{
private:
	static server* server_;
	static database* database_;
public:
	static bool init_server(const std::string& host, unsigned short int port);
	static bool init_server(const char* certificate, const char* privatekey, const std::string& host, unsigned short int port);
	static bool init_ssl_server(const char* certificate, const char* privatekey, const std::string& host, unsigned short int port);
	static bool init_database(const std::string& schema, const std::string& users, const std::string& fishing);
	static server& get_server();
	static database& get_database();
	static void destroy();
};
