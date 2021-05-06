#pragma once

#include <ctime>
#include <unordered_map>
#include <string>

#include "utils.hpp"

struct user
{
private:
	unsigned int id_;
	std::string login_;
	std::string password_;
	std::string salt_;
	long long unsigned int created_at_;
	bool banned_;
	static std::string peper_;

public:
	user();
	user(unsigned int id, std::string login, std::string password, std::string salt, unsigned long long created_at, bool banned);
	unsigned int get_id() const;
	const std::string& get_login() const;
	const std::string& get_password() const;
	const std::string& get_salt() const;
	long long unsigned int get_create_date() const;
	bool is_banned() const;
	bool set_banned(bool status);
	static std::string salt_hash(const std::string& login, const std::string& password);
	static std::string password_hash(const std::string& password, const std::string& salt);
	static std::string login_password_hash(const std::string& login, const std::string& password);
	bool is_valid_password(const std::string& password) const;
	static const std::string& get_peper();
	static void set_peper(const std::string& peper);
	bool equals(const user& user) const;
};