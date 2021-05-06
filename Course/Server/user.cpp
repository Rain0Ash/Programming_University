// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "user.hpp"

#include <utility>
#include "sha256.hpp"

std::string user::peper_;

user::user(): id_(0), created_at_(0), banned_(false)
{
}

user::user(const unsigned id, std::string login, std::string password, std::string salt,
		   const unsigned long long created_at, const bool banned): id_(id), login_(std::move(login)), password_(std::move(password)),
																	salt_(std::move(salt)), created_at_(created_at), banned_(banned)
{
}

unsigned user::get_id() const
{
	return this->id_;
}

const std::string& user::get_login() const
{
	return this->login_;
}

const std::string& user::get_password() const
{
	return this->password_;
}

const std::string& user::get_salt() const
{
	return this->salt_;
}

unsigned long long user::get_create_date() const
{
	return this->created_at_;
}

bool user::is_banned() const
{
	return this->banned_;
}

bool user::set_banned(const bool status)
{
	if (is_banned() == status)
	{
		return status;
	}

	this->banned_ = status;
	return is_banned();
}

std::string user::salt_hash(const std::string& login, const std::string& password)
{
	sha256 sha = sha256() << get_peper() << login << get_peper() << password << get_peper();
	return sha.to_string();
}

std::string user::password_hash(const std::string& password, const std::string& salt)
{
	sha256 sha = sha256() << password << get_peper() << salt;
	return sha.to_string();
}

std::string user::login_password_hash(const std::string& login, const std::string& password)
{
	const std::string salt = salt_hash(login, password);
	return password_hash(password, salt);
}

bool user::is_valid_password(const std::string& password) const
{
	return login_password_hash(this->get_login(), password) == this->get_password();
}

const std::string& user::get_peper()
{
	return peper_;
}

void user::set_peper(const std::string& peper)
{
	peper_ = peper;
}

bool user::equals(const user& user) const
{
	return
		this->get_id() == user.get_id() &&
		this->get_login() == user.get_login() &&
		this->get_password() == user.get_password() &&
		this->get_salt() == user.get_salt() &&
		this->get_create_date() == user.get_create_date() &&
		this->is_banned() == user.is_banned();
}
