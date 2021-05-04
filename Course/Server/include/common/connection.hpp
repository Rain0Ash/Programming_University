#pragma once

#include <string>

#include "user.hpp"

class connection final
{
private:
	static std::string get_apikey(const user& user, uint64_t connection_time);
	
	std::string apikey_;
	user user_;
	uint64_t connection_time_ = 0;
public:
	connection();
	explicit connection(user user);
	const std::string& get_apikey() const;
	user& get_user();
	const uint64_t& get_connection_time() const;
};