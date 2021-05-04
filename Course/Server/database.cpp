// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "database.hpp"

database* database::current_ = nullptr;

bool database::connect_to_database(SAConnection* connection, const char* database, const char* username, const char* password)
{
	try
	{
		if (connection == nullptr)
		{
			return false;
		}

		if (connection->isConnected())
		{
			return true;
		}

		connection->Connect(_TSA(database), _TSA(username), _TSA(password), SA_SQLServer_Client);
		return true;
	}
	catch (SAException& _)
	{
		return false;
	}
}

bool database::initialize_sqlapi_connection(SAConnection* connection, const char* database, const char* username, const char* password, const bool patched)
{
	if (connection == nullptr)
	{
		return false;
	}

	static bool initialized = false;

	if (initialized)
	{
		return connect_to_database(connection, database, username, password);
	}

	if (!connect_to_database(connection, database, username, password))
	{
		return false;
	}

	initialized = true;
	return true;
}

bool database::log_sqlapi_exception(SAException& exception, const message_type& type)
{
	return logger::log(exception.ErrText().GetMultiByteChars(), type);
}

bool database::get_current_database(database& database)
{
	if (current_ == nullptr)
	{
		return false;
	}

	database = *current_;
	return true;
}

database* database::get_current_database()
{
	return current_;
}

bool database::set_current_database(database* database)
{
	if (database == nullptr)
	{
		return false;
	}

	current_ = database;
	return true;
}

database::database(const std::string& schema, const std::string& users, const std::string& fishing)
{
	this->connection_ = new SAConnection();
	this->schema_ = schema;
	this->users_ = users;
	this->fishing_ = fishing;
}

bool database::connect_to_database(const char* database, const char* username, const char* password) const
{
	return initialize_sqlapi_connection(this->connection_, database, username, password);
}

bool database::create_schema() const
{
	try
	{
		constexpr const char* create_schema_command_template =
			"IF NOT EXISTS (SELECT * FROM sys.schemas WHERE name = N'{0}') EXEC('CREATE SCHEMA [{0}]');";

		std::string string = std::string(create_schema_command_template);
		utils::replace(string, "{0}", this->schema_);

		SACommand command(this->connection_, string.c_str(), SA_CmdSQLStmtRaw);
		command.Execute();

		return true;
	}
	catch (SAException& exception)
	{
		log_sqlapi_exception(exception, message_type::fatal);
		return false;
	}
}

bool database::create_users_table() const
{
	try
	{
		constexpr const char* create_users_table_command_template =
			"IF NOT EXISTS (SELECT * FROM sys.tables tables JOIN sys.schemas schemas ON (tables.schema_id = schemas.schema_id) WHERE schemas.name = '{0}' AND tables.name = '{1}') CREATE TABLE {0}.{1} (Id int IDENTITY(1,1) PRIMARY KEY, Login varchar(32) NOT NULL UNIQUE, Password varchar(64), Salt varchar(64), CreatedAt datetime NOT NULL DEFAULT GETUTCDATE(), Banned bit NOT NULL DEFAULT 0);";

		std::string string = std::string(create_users_table_command_template);
		utils::replace(string, "{0}", this->schema_);
		utils::replace(string, "{1}", this->users_);

		SACommand command(this->connection_, string.c_str(), SA_CmdSQLStmtRaw);

		command.Execute();

		return true;
	}
	catch (SAException& exception)
	{
		log_sqlapi_exception(exception, message_type::fatal);
		return false;
	}
}

bool database::create_fishing_table() const
{
	try
	{
		constexpr const char* create_fishing_table_command_template =
			"IF NOT EXISTS (SELECT * FROM sys.tables tables JOIN sys.schemas schemas ON (tables.schema_id = schemas.schema_id) WHERE schemas.name = '{0}' AND tables.name = '{1}') CREATE TABLE {0}.{1} (Id bigint IDENTITY(1,1) PRIMARY KEY, UserId int NOT NULL, Count int NOT NULL DEFAULT 1, CreatedAt datetime DEFAULT GETUTCDATE(), Invalid bit NOT NULL DEFAULT 0);";

		std::string string = std::string(create_fishing_table_command_template);
		utils::replace(string, "{0}", this->schema_);
		utils::replace(string, "{1}", this->fishing_);

		SACommand command(this->connection_, string.c_str(), SA_CmdSQLStmtRaw);

		command.Execute();

		return true;
	}
	catch (SAException& exception)
	{
		log_sqlapi_exception(exception, message_type::fatal);
		return false;
	}
}

user database::get_user(const std::string& login) const
{
	try
	{
		constexpr const char* get_user_command_template =
			"SELECT {2}.Id, {2}.Login, {2}.Password, {2}.Salt, datediff(SECOND,{d '1970-01-01'}, {2}.CreatedAt), {2}.Banned FROM {1}.{2} WHERE {2}.Login = '{0}';";

		std::string string = std::string(get_user_command_template);
		utils::replace(string, "{0}", login);
		utils::replace(string, "{1}", this->schema_);
		utils::replace(string, "{2}", this->users_);

		SACommand command(this->connection_, string.c_str(), SA_CmdSQLStmtRaw);

		command.Execute();

		if (command.FetchNext())
		{
			return user
			(
				(unsigned int)command[1].asULong(),
				std::string(command[2].asString().GetMultiByteChars()),
				std::string(command[3].asString().GetMultiByteChars()),
				std::string(command[4].asString().GetMultiByteChars()),
				command[5].asUInt64(),
				command[6].asBool()
			);
		}

		return user();
	}
	catch (SAException& exception)
	{
		log_sqlapi_exception(exception, message_type::error);
		return user();
	}
}

user database::register_user(const std::string& login, const std::string& password) const
{
	try
	{
		constexpr const char* register_user_command_template =
			"IF NOT EXISTS(SELECT {4}.Id FROM {3}.{4} WHERE {4}.Login = '{0}') BEGIN INSERT INTO {3}.{4} ({4}.Login, {4}.Password, {4}.Salt) VALUES ('{0}', '{1}', '{2}'); END SELECT {4}.Id, {4}.Login, {4}.Password, {4}.Salt, datediff(SECOND,{d '1970-01-01'}, {4}.CreatedAt), {4}.Banned FROM {3}.{4} WHERE {4}.Login = '{0}';";

		const std::string salt = user::salt_hash(login, password);

		std::string string = std::string(register_user_command_template);
		utils::replace(string, "{0}", login);
		utils::replace(string, "{1}", user::password_hash(password, salt));
		utils::replace(string, "{2}", salt);
		utils::replace(string, "{3}", this->schema_);
		utils::replace(string, "{4}", this->users_);

		SACommand command(this->connection_, string.c_str(), SA_CmdSQLStmtRaw);

		command.Execute();

		if (command.FetchNext())
		{
			return user
			(
				(unsigned int)command[1].asULong(),
				std::string(command[2].asString().GetMultiByteChars()),
				std::string(command[3].asString().GetMultiByteChars()),
				std::string(command[4].asString().GetMultiByteChars()),
				command[5].asUInt64(),
				command[6].asBool()
			);
		}

		return user();
	}
	catch (SAException& exception)
	{
		log_sqlapi_exception(exception, message_type::error);
		return user();
	}
}

bool database::register_fishing_transaction(const std::string& login) const
{
	return register_fishing_transaction(login, 1);
}

bool database::register_fishing_transaction(const std::string& login, const unsigned count) const
{
	try
	{
		constexpr const char* register_user_command_template =
			"DECLARE @Id INT; SET @Id = (SELECT TOP 1 {2}.{3}.Id FROM {2}.{3} WHERE {2}.{3}.Login = '{0}'); IF @Id IS NOT NULL INSERT INTO {2}.{4} ({4}.UserId, {4}.Count) VALUES (@Id, {1});";

		std::string string = std::string(register_user_command_template);
		utils::replace(string, "{0}", login);
		utils::replace(string, "{1}", std::to_string(count));
		utils::replace(string, "{2}", this->schema_);
		utils::replace(string, "{3}", this->users_);
		utils::replace(string, "{4}", this->fishing_);

		SACommand command(this->connection_, string.c_str(), SA_CmdSQLStmtRaw);

		command.Execute();

		return true;
	}
	catch (SAException& exception)
	{
		log_sqlapi_exception(exception, message_type::error);
		return false;
	}
}

bool database::register_fishing_transaction(const unsigned userid) const
{
	return register_fishing_transaction(userid, 1);
}

bool database::register_fishing_transaction(const unsigned userid, const unsigned count) const
{
	try
	{
		constexpr const char* register_user_command_template =
			"INSERT INTO {2}.{3} ({3}.UserId, {3}.Count) VALUES ({0}, {1});";

		std::string string = std::string(register_user_command_template);
		utils::replace(string, "{0}", std::to_string(userid));
		utils::replace(string, "{1}", std::to_string(count));
		utils::replace(string, "{2}", this->schema_);
		utils::replace(string, "{3}", this->fishing_);

		SACommand command(this->connection_, string.c_str(), SA_CmdSQLStmtRaw);

		command.Execute();

		return true;
	}
	catch (SAException& exception)
	{
		log_sqlapi_exception(exception, message_type::error);
		return false;
	}
}

bool database::get_fishing_statistics(const unsigned userid, const time_t start, const time_t end, const unsigned skip, const unsigned take, std::vector<fishing_entry>& vector) const
{
	try
	{
		constexpr const char* get_statistics_command_template =
			"SELECT {5}.Id, {5}.Count, datediff(SECOND,{d '1970-01-01'}, {5}.CreatedAt) AS Date FROM {3}.{5} LEFT JOIN {3}.{4} ON {4}.Id = {5}.UserId WHERE {4}.Banned = 0 AND {5}.UserId = {0} AND {5}.Invalid = 0 AND {5}.CreatedAt BETWEEN dateadd(S, {1}, '1970-01-01') AND dateadd(S, {2}, '1970-01-01') ORDER BY Date DESC OFFSET {6} ROWS FETCH NEXT {7} ROWS ONLY;";

		std::string string = std::string(get_statistics_command_template);
		utils::replace(string, "{0}", std::to_string(userid));
		utils::replace(string, "{1}", std::to_string(start));
		utils::replace(string, "{2}", std::to_string(end));
		utils::replace(string, "{3}", this->schema_);
		utils::replace(string, "{4}", this->users_);
		utils::replace(string, "{5}", this->fishing_);
		utils::replace(string, "{6}", std::to_string(skip));
		utils::replace(string, "{7}", std::to_string(take));

		SACommand command(this->connection_, string.c_str(), SA_CmdSQLStmtRaw);

		command.Execute();

		while (command.FetchNext())
		{
			vector.emplace_back
			(
				command[1].asULong(),
				command[2].asULong(),
				command[3].asULong()
			);
		}

		return true;
	}
	catch (SAException& exception)
	{
		log_sqlapi_exception(exception, message_type::error);
		return false;
	}
}

bool database::get_fishing_records(const time_t start, const time_t end,
                                   const unsigned skip,
                                   const unsigned take, std::vector<fishing_record_entry>& vector) const
{
	try
	{
		constexpr const char* get_statistics_command_template =
			"SELECT {3}.Login, SUM({4}.Count) AS Count FROM {2}.{4} LEFT JOIN {2}.{3} ON {3}.Id = {4}.UserId WHERE {3}.Banned = 0 AND {4}.Invalid = 0 AND {4}.CreatedAt BETWEEN dateadd(S, {0}, '1970-01-01') AND dateadd(S, {1}, '1970-01-01') GROUP BY {3}.Login ORDER BY Count DESC OFFSET {5} ROWS FETCH NEXT {6} ROWS ONLY;";

		std::string string = std::string(get_statistics_command_template);
		utils::replace(string, "{0}", std::to_string(start));
		utils::replace(string, "{1}", std::to_string(end));
		utils::replace(string, "{2}", this->schema_);
		utils::replace(string, "{3}", this->users_);
		utils::replace(string, "{4}", this->fishing_);
		utils::replace(string, "{5}", std::to_string(skip));
		utils::replace(string, "{6}", std::to_string(take));

		SACommand command(this->connection_, string.c_str(), SA_CmdSQLStmtRaw);

		command.Execute();

		while (command.FetchNext())
		{
			vector.emplace_back
			(
				std::string(command[1].asString().GetMultiByteChars()),
				command[2].asULong()
			);
		}

		return true;
	}
	catch (SAException& exception)
	{
		log_sqlapi_exception(exception, message_type::error);
		return false;
	}
}

bool database::get_fishing_records(const unsigned userid, const time_t start, const time_t end, std::vector<fishing_record_entry>& vector) const
{
	try
	{
		constexpr const char* get_statistics_command_template =
			"SELECT {3}.Login, SUM({4}.Count) AS Count FROM {2}.{4} LEFT JOIN {2}.{3} ON {3}.Id = {4}.UserId WHERE {3}.Id = '{5}' AND {3}.Banned = 0 AND {4}.Invalid = 0 AND {4}.CreatedAt BETWEEN dateadd(S, {0}, '1970-01-01') AND dateadd(S, {1}, '1970-01-01') GROUP BY {3}.Login;";

		std::string string = std::string(get_statistics_command_template);
		utils::replace(string, "{0}", std::to_string(start));
		utils::replace(string, "{1}", std::to_string(end));
		utils::replace(string, "{2}", this->schema_);
		utils::replace(string, "{3}", this->users_);
		utils::replace(string, "{4}", this->fishing_);
		utils::replace(string, "{5}", std::to_string(userid));

		SACommand command(this->connection_, string.c_str(), SA_CmdSQLStmtRaw);

		command.Execute();

		while (command.FetchNext())
		{
			vector.emplace_back
			(
				std::string(command[1].asString().GetMultiByteChars()),
				command[2].asULong()
			);
		}

		return true;
	}
	catch (SAException& exception)
	{
		log_sqlapi_exception(exception, message_type::error);
		return false;
	}
}

bool database::set_ban_status(const unsigned userid) const
{
	return set_ban_status(userid, true);
}

bool database::set_ban_status(const unsigned userid, const bool status) const
{
	try
	{
		constexpr const char* set_ban_status_command_template =
			"UPDATE {2}.{3} SET {3}.Banned = {1} WHERE {3}.Id = {0};";

		std::string string = std::string(set_ban_status_command_template);
		utils::replace(string, "{0}", std::to_string(userid));
		utils::replace(string, "{1}", std::to_string(status));
		utils::replace(string, "{2}", this->schema_);
		utils::replace(string, "{3}", this->users_);

		SACommand command(this->connection_, string.c_str(), SA_CmdSQLStmtRaw);

		command.Execute();

		return true;
	}
	catch (SAException& exception)
	{
		log_sqlapi_exception(exception, message_type::error);
		return false;
	}
}

bool database::set_ban_status(const std::string& login) const
{
	return set_ban_status(login, true);
}

bool database::set_ban_status(const std::string& login, const bool status) const
{
	try
	{
		constexpr const char* set_ban_status_command_template =
			"UPDATE {2}.{3} SET {3}.Banned = {1} WHERE {3}.Login = '{0}';";

		std::string string = std::string(set_ban_status_command_template);
		utils::replace(string, "{0}", login);
		utils::replace(string, "{1}", std::to_string(status));
		utils::replace(string, "{2}", this->schema_);
		utils::replace(string, "{3}", this->users_);

		SACommand command(this->connection_, string.c_str(), SA_CmdSQLStmtRaw);

		command.Execute();

		return true;
	}
	catch (SAException& exception)
	{
		log_sqlapi_exception(exception, message_type::error);
		return false;
	}
}

bool database::set_ban_status(const user& user) const
{
	return set_ban_status(user, true);
}

bool database::set_ban_status(const user& user, const bool status) const
{
	return set_ban_status(user.get_id(), status);
}

bool database::rollback() const
{
	try
	{
		this->connection_->Rollback();
		return true;
	}
	catch (SAException& exception)
	{
		log_sqlapi_exception(exception, message_type::error);
		return false;
	}
}

database::~database()
{
	connection_->Disconnect();
	delete this->connection_;
}
