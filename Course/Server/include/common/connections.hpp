#pragma once

#include "connection.hpp"

class connections final
{
private:
	static std::unordered_map<std::string, connection*>* connections_by_apikey_;
	static std::unordered_map<std::string, connection*>* connections_by_login_;
public:
	connections() = delete;
	static bool get_connection(const std::string& apikey, connection& connection);
	static bool get_connection_by_login(const std::string& login, connection& connection);
	static connection create_connection(const user& user);
	static bool remove_connection(const user& user);
	static bool remove_connection(const std::string& apikey);
	static bool remove_connection_by_login(const std::string& login);
};
