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

bool database::initialize_sqlapi_connection(SAConnection* connection, const char* database, const char* username, const char* password)
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
			"IF NOT EXISTS (SELECT * FROM sys.schemas WHERE name = N'{schema}') EXEC('CREATE SCHEMA [{schema}]');";

		SACommand command(this->connection_, insert_database_information(create_schema_command_template).c_str(), SA_CmdSQLStmtRaw);
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
			"IF NOT EXISTS (SELECT * FROM sys.tables tables JOIN sys.schemas schemas ON (tables.schema_id = schemas.schema_id) WHERE schemas.name = '{schema}' AND tables.name = '{users}') CREATE TABLE {schema}.{users} (Id INT IDENTITY(1,1) PRIMARY KEY, Login VARCHAR(64) NOT NULL UNIQUE, Password VARCHAR(64), Salt VARCHAR(64), CreatedAt DATETIME NOT NULL DEFAULT GETUTCDATE(), Banned BIT NOT NULL DEFAULT 0);";

		SACommand command(this->connection_, insert_database_information(create_users_table_command_template).c_str(), SA_CmdSQLStmtRaw);
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
			"IF NOT EXISTS (SELECT * FROM sys.tables tables JOIN sys.schemas schemas ON (tables.schema_id = schemas.schema_id) WHERE schemas.name = '{schema}' AND tables.name = '{fishing}') CREATE TABLE {schema}.{fishing} (Id BIGINT IDENTITY(1,1) PRIMARY KEY, UserId INT NOT NULL, Count INT NOT NULL DEFAULT 1, CreatedAt DATETIME DEFAULT GETUTCDATE(), Invalid BIT NOT NULL DEFAULT 0);";

		SACommand command(this->connection_, insert_database_information(create_fishing_table_command_template).c_str(), SA_CmdSQLStmtRaw);
		command.Execute();

		return true;
	}
	catch (SAException& exception)
	{
		log_sqlapi_exception(exception, message_type::fatal);
		return false;
	}
}

std::string database::insert_database_information(const std::string& string) const
{
	std::string value = string;
	utils::replace(value, "{schema}", this->schema_);
	utils::replace(value, "{users}", this->users_);
	utils::replace(value, "{fishing}", this->fishing_);

	return value;
}

void database::register_stored_procedure(const std::string& name, const std::string& params, const std::string& sql) const
{
	constexpr const char* procedure_template = "CREATE OR ALTER PROCEDURE dbo.{0} {1} AS {2};";

	std::string procedure = std::string(procedure_template);
	utils::replace(procedure, "{0}", name);
	utils::replace(procedure, "{1}", params);
	utils::replace(procedure, "{2}", sql);

	SACommand command(this->connection_, procedure.c_str(), SA_CmdSQLStmtRaw);
	command.Execute();
}

bool database::register_stored_procedure() const
{
	try
	{
		register_stored_procedure("GetUser", "@Login VARCHAR(64)", insert_database_information("SELECT {users}.Id, {users}.Login, {users}.Password, {users}.Salt, datediff(SECOND,{d '1970-01-01'}, {users}.CreatedAt) AS CreatedAt, {users}.Banned FROM {schema}.{users} WHERE {users}.Login = @Login"));
		register_stored_procedure("RegisterUser", "@Login VARCHAR(64), @Password VARCHAR(64), @Salt VARCHAR(64)", insert_database_information("IF NOT EXISTS(SELECT {users}.Id FROM {schema}.{users} WHERE {users}.Login = @Login) BEGIN INSERT INTO {schema}.{users} ({users}.Login, {users}.Password, {users}.Salt) VALUES (@Login, @Password, @Salt); END SELECT {users}.Id, {users}.Login, {users}.Password, {users}.Salt, datediff(SECOND,{d '1970-01-01'}, {users}.CreatedAt), {users}.Banned FROM {schema}.{users} WHERE {users}.Login = @Login"));
		register_stored_procedure("RegisterFishingLogin", "@Login VARCHAR(64), @Count INT", insert_database_information("DECLARE @Id INT; SET @Id = (SELECT TOP 1 {schema}.{users}.Id FROM {schema}.{users} WHERE {schema}.{users}.Login = @Login); IF @Id IS NOT NULL INSERT INTO {schema}.{fishing} ({fishing}.UserId, {fishing}.Count) VALUES (@Id, @Count)"));
		register_stored_procedure("RegisterFishingUserId", "@UserId INT, @Count INT", insert_database_information("INSERT INTO {schema}.{fishing} ({fishing}.UserId, {fishing}.Count) VALUES (@UserId, @Count)"));
		register_stored_procedure("GetFishingStatistics", "@UserId INT, @Start INT, @End INT, @Skip INT, @Take INT", insert_database_information("SELECT {fishing}.Id, {fishing}.Count, datediff(SECOND,{d '1970-01-01'}, {fishing}.CreatedAt) AS Date FROM {schema}.{fishing} LEFT JOIN {schema}.{users} ON {users}.Id = {fishing}.UserId WHERE {users}.Banned = 0 AND {fishing}.UserId = @UserId AND {fishing}.Invalid = 0 AND {fishing}.CreatedAt BETWEEN dateadd(S, @Start, '1970-01-01') AND dateadd(S, @End, '1970-01-01') ORDER BY Date DESC OFFSET @Skip ROWS FETCH NEXT @Take ROWS ONLY"));
		register_stored_procedure("GetFishingRecords", "@Start INT, @End INT, @Skip INT, @Take INT", insert_database_information("SELECT {users}.Login, SUM({fishing}.Count) AS Count FROM {schema}.{fishing} LEFT JOIN {schema}.{users} ON {users}.Id = {fishing}.UserId WHERE {users}.Banned = 0 AND {fishing}.Invalid = 0 AND {fishing}.CreatedAt BETWEEN dateadd(S, @Start, '1970-01-01') AND dateadd(S, @End, '1970-01-01') GROUP BY {users}.Login ORDER BY Count DESC OFFSET @Skip ROWS FETCH NEXT @Take ROWS ONLY"));
		register_stored_procedure("GetFishingRecordsUserId", "@UserId INT, @Start INT, @End INT", insert_database_information("SELECT {users}.Login, SUM({fishing}.Count) AS Count FROM {schema}.{fishing} LEFT JOIN {schema}.{users} ON {users}.Id = {fishing}.UserId WHERE {users}.Id = @UserId AND {users}.Banned = 0 AND {fishing}.Invalid = 0 AND {fishing}.CreatedAt BETWEEN dateadd(S, @Start, '1970-01-01') AND dateadd(S, @End, '1970-01-01') GROUP BY {users}.Login"));
		register_stored_procedure("SetBanStatusLogin", "@Login VARCHAR(64), @Banned BIT", insert_database_information("UPDATE {schema}.{users} SET {users}.Banned = @Banned WHERE {users}.Login = @Login"));
		register_stored_procedure("SetBanStatusUserId", "@UserId INT, @Banned BIT", insert_database_information("UPDATE {schema}.{users} SET {users}.Banned = @Banned WHERE {users}.Id = @UserId"));
		
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
		constexpr const char* get_user_command_template = "EXECUTE GetUser @Login = '{0}';";

		std::string string = std::string(get_user_command_template);
		utils::replace(string, "{0}", login);

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
		constexpr const char* register_user_command_template = "EXECUTE RegisterUser @Login = '{0}', @Password = '{1}', @Salt = '{2}';";

		const std::string salt = user::salt_hash(login, password);

		std::string string = std::string(register_user_command_template);
		utils::replace(string, "{0}", login);
		utils::replace(string, "{1}", user::password_hash(password, salt));
		utils::replace(string, "{2}", salt);

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
		constexpr const char* register_fishing_transaction_command_template = "EXECUTE RegisterFishingLogin @Login = '{0}', @Count = {1};";

		std::string string = std::string(register_fishing_transaction_command_template);
		utils::replace(string, "{0}", login);
		utils::replace(string, "{1}", std::to_string(count));

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
		constexpr const char* register_fishing_transaction_command_template = "EXECUTE RegisterFishingUserId @UserId = {0}, @Count = {1};";

		std::string string = std::string(register_fishing_transaction_command_template);
		utils::replace(string, "{0}", std::to_string(userid));
		utils::replace(string, "{1}", std::to_string(count));

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
		constexpr const char* get_statistics_command_template = "EXECUTE GetFishingStatistics @UserId = {0}, @Start = {1}, @End = {2}, @Skip = {3}, @Take = {4};";

		std::string string = std::string(get_statistics_command_template);
		utils::replace(string, "{0}", std::to_string(userid));
		utils::replace(string, "{1}", std::to_string(start));
		utils::replace(string, "{2}", std::to_string(end));
		utils::replace(string, "{3}", std::to_string(skip));
		utils::replace(string, "{4}", std::to_string(take));

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

bool database::get_fishing_records(const time_t start, const time_t end, const unsigned skip, const unsigned take, std::vector<fishing_record_entry>& vector) const
{
	try
	{
		constexpr const char* get_statistics_command_template = "EXECUTE GetFishingRecords @Start = {0}, @End = {1}, @Skip = {2}, @Take = {3};";

		std::string string = std::string(get_statistics_command_template);
		utils::replace(string, "{0}", std::to_string(start));
		utils::replace(string, "{1}", std::to_string(end));
		utils::replace(string, "{2}", std::to_string(skip));
		utils::replace(string, "{3}", std::to_string(take));

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
		constexpr const char* get_statistics_command_template = "EXECUTE GetFishingRecordsUserId @UserId = {0}, @Start = {1}, @End = {2};";

		std::string string = std::string(get_statistics_command_template);
		utils::replace(string, "{0}", std::to_string(userid));
		utils::replace(string, "{1}", std::to_string(start));
		utils::replace(string, "{2}", std::to_string(end));

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
		constexpr const char* set_ban_status_command_template = "EXECUTE SetBanStatusUserId @UserId = {0}, @Banned = {1};";

		std::string string = std::string(set_ban_status_command_template);
		utils::replace(string, "{0}", std::to_string(userid));
		utils::replace(string, "{1}", std::to_string(status));

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
		constexpr const char* set_ban_status_command_template = "EXECUTE SetBanStatusLogin @Login = '{0}', @Banned = {1};";

		std::string string = std::string(set_ban_status_command_template);
		utils::replace(string, "{0}", login);
		utils::replace(string, "{1}", std::to_string(status));

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
