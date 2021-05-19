#pragma once

#include "SQLAPI.h"
#include <string>

#include "logger.hpp"
#include "user.hpp"

#include "fishing_entry.hpp"
#include "fishing_record_entry.hpp"


class database final
{
private:
	static database* current_;
	
	SAConnection* connection_;

	std::string schema_;
	std::string users_;
	std::string fishing_;

	std::string insert_database_information(const std::string& string) const;
	void register_stored_procedure(const std::string& name, const std::string& params, const std::string& sql) const;
	
public:
	static bool connect_to_database(SAConnection* connection, const char* database, const char* username, const char* password);
	static bool initialize_sqlapi_connection(SAConnection* connection, const char* database, const char* username, const char* password);
	static bool log_sqlapi_exception(SAException& exception, const message_type& type = message_type::error);
	static bool get_current_database(database& database);
	static database* get_current_database();
	static bool set_current_database(database* database);

	database(const std::string& schema, const std::string& users, const std::string& fishing);
	bool connect_to_database(const char* database, const char* username, const char* password) const;
	bool create_schema() const;
	bool create_users_table() const;
	bool create_fishing_table() const;
	bool register_stored_procedure() const;
	user get_user(const std::string& login) const;
	user register_user(const std::string& login, const std::string& password) const;
	bool register_fishing_transaction(const std::string& login) const;
	bool register_fishing_transaction(const std::string& login, unsigned int count) const;
	bool register_fishing_transaction(unsigned int userid) const;
	bool register_fishing_transaction(unsigned int userid, unsigned int count) const;
	bool get_fishing_statistics(unsigned int userid, time_t start, time_t end, unsigned int skip, unsigned int take, std::vector<fishing_entry>& vector) const;
	bool get_fishing_records(time_t start, time_t end, unsigned int skip, unsigned int take, std::vector<fishing_record_entry>& vector) const;
	bool get_fishing_records(unsigned int userid, time_t start, time_t end, std::vector<fishing_record_entry>& vector) const;
	bool set_ban_status(unsigned int userid) const;
	bool set_ban_status(unsigned int userid, bool status) const;
	bool set_ban_status(const std::string& login) const;
	bool set_ban_status(const std::string& login, bool status) const;
	bool set_ban_status(const user& user) const;
	bool set_ban_status(const user& user, bool status) const;
	bool rollback() const;
	~database();
};
