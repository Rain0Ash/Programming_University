// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "instance.hpp"

server* instance::server_ = nullptr;
database* instance::database_ = nullptr;

bool instance::init_server(const std::string& host, const unsigned short port)
{
	if (server_)
	{
		return false;
	}

	server_ = new server(host, port);
	return true;
}

bool instance::init_server(const char* certificate, const char* privatekey, const std::string& host, const unsigned short port)
{
	if (server_)
	{
		return false;
	}

	server_ = server::is_certificate_installed(certificate, privatekey)
				  ? new server(certificate, privatekey, host, port)
				  : new server(host, port);
	return true;
}

bool instance::init_ssl_server(const char* certificate, const char* privatekey, const std::string& host, const unsigned short port)
{
	if (server_)
	{
		return false;
	}

	if (!server::is_certificate_installed(certificate, privatekey))
	{
		return false;
	}

	server_ = new server(certificate, privatekey, host, port);
	return true;
}

bool instance::init_database(const std::string& schema, const std::string& users, const std::string& fishing)
{
	if (database_)
	{
		return false;
	}

	database_ = new database(schema, users, fishing);
	database::set_current_database(database_);
	return true;
}

server& instance::get_server()
{
	if (server_)
	{
		return *server_;
	}

	throw std::exception();
}

database& instance::get_database()
{
	if (server_)
	{
		return *database_;
	}

	throw std::exception();
}

void instance::destroy()
{
	delete server_;
	delete database_;
}
