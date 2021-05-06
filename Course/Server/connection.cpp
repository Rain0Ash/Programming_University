// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "connection.hpp"
#include "sha256.hpp"

std::string connection::get_apikey(const user& user, const uint64_t connection_time)
{
	sha256 sha = sha256() << std::to_string(user.get_id()) << user.get_login() << std::to_string(connection_time) << std::to_string(rand());
	return sha.to_string();
}

connection::connection() = default;

connection::connection(user user): user_(std::move(user))
{
	connection_time_ = utils::time_epoch();
	apikey_ = get_apikey(user_, connection_time_);
}

const std::string& connection::get_apikey() const
{
	return this->apikey_;
}

user& connection::get_user()
{
	return this->user_;
}

const uint64_t& connection::get_connection_time() const
{
	return this->connection_time_;
}
