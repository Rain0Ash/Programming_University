#pragma once

#include <ctime>
#include <unordered_map>
#include <string>

#include "SHA256Impl.hpp"
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
	user(): id_(0), created_at_(0), banned_(false)
	{
	}

	user(const unsigned int id, const std::string& login, const std::string& password, const std::string& salt, const unsigned long long created_at, const bool banned)
		: id_(id), login_(login), password_(password), salt_(salt), created_at_(created_at), banned_(banned)
	{
	}

	unsigned int get_id() const
	{
		return this->id_;
	}

	const std::string& get_login() const
	{
		return this->login_;
	}

	const std::string& get_password() const
	{
		return this->password_;
	}

	const std::string& get_salt() const
	{
		return this->salt_;
	}

	long long unsigned int get_create_date() const
	{
		return this->created_at_;
	}

	bool is_banned() const
	{
		return this->banned_;
	}

	bool set_banned(bool status)
	{
		if (is_banned() == status)
		{
			return status;
		}

		this->banned_ = status;
		return is_banned();
	}

	static std::string salt_hash(const std::string& login, const std::string& password)
	{
		return SHA256Impl().update(get_peper()).update(login).update(get_peper()).update(password).update(get_peper()).to_string();
	}

	static std::string password_hash(const std::string& password, const std::string& salt)
	{
		return SHA256Impl().update(password).update(get_peper()).update(salt).to_string();
	}

	static std::string login_password_hash(const std::string& login, const std::string& password)
	{
		const std::string salt = salt_hash(login, password);
		return password_hash(password, salt);
	}

	bool is_valid_password(const std::string& password) const
	{
		return login_password_hash(this->get_login(), password) == this->get_password();
	}

	static const std::string& get_peper()
	{
		return peper_;
	}

	static void set_peper(const std::string& peper)
	{
		peper_ = peper;
	}

	bool equals(const user& user) const
	{
		return
			this->get_id() == user.get_id() &&
			this->get_login() == user.get_login() &&
			this->get_password() == user.get_password() &&
			this->get_salt() == user.get_salt() &&
			this->get_create_date() == user.get_create_date() &&
			this->is_banned() == user.is_banned();
	}
};

class connection final
{
private:
	std::string apikey;
	user user_;
	uint64_t connection_time_;

	static std::string get_apikey(const user& user, const uint64_t connection_time)
	{
		return SHA256Impl().update(std::to_string(user.get_id())).update(user.get_login()).update(std::to_string(connection_time)).update(std::to_string(rand())).to_string();
	}
public:
	connection() {}

	connection(user user) : user_(std::move(user))
	{
		connection_time_ = time_epoch();
		apikey = get_apikey(user_, connection_time_);
	}

	const std::string& get_apikey() const
	{
		return this->apikey;
	}

	user& get_user()
	{
		return this->user_;
	}

	const uint64_t& get_connection_time() const
	{
		return this->connection_time_;
	}
};

class connections final
{
private:
	static std::unordered_map<std::string, connection*>* connections_by_apikey_;
	static std::unordered_map<std::string, connection*>* connections_by_login_;
public:
	connections() = delete;

	static bool get_connection(const std::string& apikey, connection& connection)
	{
		if (connections_by_apikey_->find(apikey) == connections_by_apikey_->end())
		{
			return false;
		}

		connection = *connections_by_apikey_->at(apikey);
		return true;
	}

	static bool get_connection_by_login(const std::string& login, connection& connection)
	{
		if (connections_by_login_->find(login) == connections_by_login_->end())
		{
			return false;
		}

		connection = *connections_by_login_->at(login);
		return true;
	}

	static connection create_connection(const user& user)
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
					connections_by_login_->insert({ mapconn->get_user().get_login(), mapconn });
					return *mapconn;
				}

				delete mapconn;
				continue;
			}

			connections_by_apikey_->insert({ mapconn->get_apikey(), mapconn });
			connections_by_login_->insert({ mapconn->get_user().get_login(), mapconn });
			return *mapconn;
		} while (true);
	}

	static bool remove_connection(const user& user)
	{
		return remove_connection_by_login(user.get_login());
	}

	static bool remove_connection(const std::string& apikey)
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

	static bool remove_connection_by_login(const std::string& login)
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
};