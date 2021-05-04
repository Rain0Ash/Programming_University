// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "connections.hpp"

std::unordered_map<std::string, connection*>* connections::connections_by_apikey_ = new std::unordered_map<std::string, connection*>;
std::unordered_map<std::string, connection*>* connections::connections_by_login_ = new std::unordered_map<std::string, connection*>;

bool connections::get_connection(const std::string& apikey, connection& connection)
{
	if (connections_by_apikey_->find(apikey) == connections_by_apikey_->end())
	{
		return false;
	}

	connection = *connections_by_apikey_->at(apikey);
	return true;
}

bool connections::get_connection_by_login(const std::string& login, connection& connection)
{
	if (connections_by_login_->find(login) == connections_by_login_->end())
	{
		return false;
	}

	connection = *connections_by_login_->at(login);
	return true;
}

connection connections::create_connection(const user& user)
{
	connection conn;
	if (get_connection_by_login(user.get_login(), conn))
	{
		return conn;
	}

	do
	{
		connection* mapconn = new connection(user);

		if (get_connection(mapconn->get_apikey(), conn))
		{
			if (conn.get_user().equals(mapconn->get_user()))
			{
				connections_by_login_->insert({mapconn->get_user().get_login(), mapconn});
				return *mapconn;
			}

			delete mapconn;
			continue;
		}

		connections_by_apikey_->insert({mapconn->get_apikey(), mapconn});
		connections_by_login_->insert({mapconn->get_user().get_login(), mapconn});
		return *mapconn;
	}
	while (true);
}

bool connections::remove_connection(const user& user)
{
	return remove_connection_by_login(user.get_login());
}

bool connections::remove_connection(const std::string& apikey)
{
	if (connections_by_apikey_->find(apikey) == connections_by_apikey_->end())
	{
		return false;
	}

	connection* connection = connections_by_apikey_->at(apikey);
	connections_by_apikey_->erase(apikey);
	connections_by_login_->erase(connection->get_user().get_login());

	delete connection;
	return true;
}

bool connections::remove_connection_by_login(const std::string& login)
{
	if (connections_by_login_->find(login) == connections_by_login_->end())
	{
		return false;
	}

	connection* connection = connections_by_login_->at(login);
	connections_by_login_->erase(login);
	connections_by_apikey_->erase(connection->get_apikey());

	delete connection;
	return true;
}
