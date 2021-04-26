#pragma once

#include <SQLAPI.h>
#include <string>
#include <thread>
#include <utility>

#include "sqlapi_utils.hpp"
#include "user.hpp"
#include "utils.hpp"
#include "logger.hpp"

class database;

bool connect_to_database(SAConnection* connection, const char* database, const char* username, const char* password)
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

bool initialize_sqlapi_connection(SAConnection* connection, const char* database, const char* username, const char* password, const bool patched = true)
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

#if SQL_NOT_PATCHED

	if (!patched)
	{
        std::thread kill_thread = std::thread(kill_evaluation_window);
        std::thread connect_thread = std::thread(connect_to_database, connection, database, username, password);
        connect_thread.join();
        kill_thread.join();

        initialized = true;
        return true;
	}

#endif

    std::thread connect_thread = std::thread(connect_to_database, connection, database, username, password);
    connect_thread.join();
	
    initialized = true;
    return true;
}

inline bool log_sqlapi_exception(SAException& exception, const message_type& type = message_type::error)
{
    return log(exception.ErrText().GetMultiByteChars(), type);
}

struct fishing_entry
{
private:
    unsigned long long int id_;
    unsigned int count_;
    time_t created_at_;
public:
    fishing_entry()
    {
    }
	
    fishing_entry(const unsigned long long int id, const unsigned int count, const time_t created_at) : id_(id), count_(count), created_at_(created_at)
    {
    }

    unsigned long long int get_id() const
    {
        return this->id_;
    }

	unsigned int get_count() const
    {
        return this->count_;
    }

	time_t get_create_date() const
    {
        return this->created_at_;
    }
};

struct fishing_record_entry
{
private:
    std::string login_;
    uint64_t count_;
    static std::string* records_cache_;
    static uint64_t records_cache_time_;
public:
    fishing_record_entry(std::string login, const uint64_t count) : login_(std::move(login)), count_(count)
    {
    }

    const std::string& get_login() const
    {
        return this->login_;
    }

    uint64_t get_count() const
    {
        return this->count_;
    }

	static std::string get_cache()
    {
	    if (records_cache_)
	    {
            return *records_cache_;
	    }

        return std::string();
    }

	static void set_cache(const std::string& cache)
    {
        delete records_cache_;
        records_cache_ = new std::string(cache);
        records_cache_time_ = time_epoch();
    }

	static uint64_t get_cache_time()
    {
        return records_cache_time_;
    }
};

class database final
{
private:
    SAConnection* connection_;
    static database* current_;
public:
    static bool get_current_database(database& database)
    {
        if (current_ == nullptr)
        {
            return false;
        }
    	
        database = *current_;
        return true;
    }

    static database* get_current_database()
    {
        return current_;
    }
	
    static bool set_current_database(database* database)
    {
        if (database == nullptr)
        {
	        return false;
        }
    	
        current_ = database;
        return true;
    }
	
    database()
    {
        this->connection_ = new SAConnection();
    }

    bool connect_to_database(const char* database, const char* username, const char* password, const bool patched=true) const
    {
        return initialize_sqlapi_connection(this->connection_, database, username, password, patched);
    }

	bool create_schema(const std::string& schema_name) const
	{
		try
        {
	        constexpr const char * create_schema_command_template = "IF NOT EXISTS (SELECT * FROM sys.schemas WHERE name = N'{0}') EXEC('CREATE SCHEMA [{0}]');";

            std::string string = std::string(create_schema_command_template);
            replace(string, "{0}", schema_name);
			
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

    bool create_users_table(const std::string& schema_name, const std::string& users_table_name) const
    {
	    try
        {
	        constexpr const char * create_users_table_command_template = "IF NOT EXISTS (SELECT * FROM sys.tables tables JOIN sys.schemas schemas ON (tables.schema_id = schemas.schema_id) WHERE schemas.name = '{0}' AND tables.name = '{1}') CREATE TABLE {0}.{1} (Id int IDENTITY(1,1) PRIMARY KEY, Login varchar(32) NOT NULL UNIQUE, Password varchar(64), Salt varchar(64), CreatedAt datetime NOT NULL DEFAULT GETUTCDATE(), Banned bit NOT NULL DEFAULT 0);";

            std::string string = std::string(create_users_table_command_template);
            replace(string, "{0}", schema_name);
            replace(string, "{1}", users_table_name);

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

    bool create_fishing_table(const std::string& schema_name, const std::string& fishing_table_name) const
    {
	    try
        {
	        constexpr const char * create_fishing_table_command_template = "IF NOT EXISTS (SELECT * FROM sys.tables tables JOIN sys.schemas schemas ON (tables.schema_id = schemas.schema_id) WHERE schemas.name = '{0}' AND tables.name = '{1}') CREATE TABLE {0}.{1} (Id bigint IDENTITY(1,1) PRIMARY KEY, UserId int NOT NULL, Count int NOT NULL DEFAULT 1, CreatedAt datetime DEFAULT GETUTCDATE(), Invalid bit NOT NULL DEFAULT 0);";

            std::string string = std::string(create_fishing_table_command_template);
            replace(string, "{0}", schema_name);
            replace(string, "{1}", fishing_table_name);

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

    user get_user(const std::string& login, const std::string& schema_name, const std::string& users_table_name) const
    {
        try
        {
            constexpr const char* get_user_command_template = "SELECT {2}.Id, {2}.Login, {2}.Password, {2}.Salt, datediff(SECOND,{d '1970-01-01'}, {2}.CreatedAt), {2}.Banned FROM {1}.{2} WHERE {2}.Login = '{0}';";

            std::string string = std::string(get_user_command_template);
            replace(string, "{0}", login);
            replace(string, "{1}", schema_name);
            replace(string, "{2}", users_table_name);
        	
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

	user register_user(const std::string& login, const std::string& password, const std::string& schema_name, const std::string& users_table_name) const
    {
	    try
	    {
		    constexpr const char * register_user_command_template = "IF NOT EXISTS(SELECT {4}.Id FROM {3}.{4} WHERE {4}.Login = '{0}') BEGIN INSERT INTO {3}.{4} ({4}.Login, {4}.Password, {4}.Salt) VALUES ('{0}', '{1}', '{2}'); END SELECT {4}.Id, {4}.Login, {4}.Password, {4}.Salt, datediff(SECOND,{d '1970-01-01'}, {4}.CreatedAt), {4}.Banned FROM {3}.{4} WHERE {4}.Login = '{0}';";

		    const std::string salt = user::salt_hash(login, password);
	    	
            std::string string = std::string(register_user_command_template);
            replace(string, "{0}", login);
            replace(string, "{1}", user::password_hash(password, salt));
            replace(string, "{2}", salt);
            replace(string, "{3}", schema_name);
            replace(string, "{4}", users_table_name);

	    	SACommand command(this->connection_, string.c_str(), SA_CmdSQLStmtRaw);

            command.Execute();

		    if (command.FetchNext())
		    {
                return user
                (
                    (unsigned int) command[1].asULong(),
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

    bool register_fishing_transaction(const std::string& login, const std::string& schema_name, const std::string& users_table_name, const std::string& fishing_table_name) const
    {
        return register_fishing_transaction(login, 1, schema_name, users_table_name, fishing_table_name);
    }

    bool register_fishing_transaction(const std::string& login, const unsigned int count, const std::string& schema_name, const std::string& users_table_name, const std::string& fishing_table_name) const
    {
        try
        {
            constexpr const char* register_user_command_template = "DECLARE @Id INT; SET @Id = (SELECT TOP 1 {2}.{3}.Id FROM {2}.{3} WHERE {2}.{3}.Login = '{0}'); IF @Id IS NOT NULL INSERT INTO {2}.{4} ({4}.UserId, {4}.Count) VALUES (@Id, {1});";

            std::string string = std::string(register_user_command_template);
            replace(string, "{0}", login);
            replace(string, "{1}", std::to_string(count));
            replace(string, "{2}", schema_name);
            replace(string, "{3}", users_table_name);
            replace(string, "{4}", fishing_table_name);

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

    bool register_fishing_transaction(const unsigned int userid, const std::string& schema_name, const std::string& fishing_table_name) const
    {
        return register_fishing_transaction(userid, 1, schema_name, fishing_table_name);
    }

    bool register_fishing_transaction(const unsigned int userid, const unsigned int count, const std::string& schema_name, const std::string& fishing_table_name) const
    {
        try
        {
            constexpr const char* register_user_command_template = "INSERT INTO {2}.{3} ({3}.UserId, {3}.Count) VALUES ({0}, {1});";

            std::string string = std::string(register_user_command_template);
            replace(string, "{0}", std::to_string(userid));
            replace(string, "{1}", std::to_string(count));
            replace(string, "{2}", schema_name);
            replace(string, "{3}", fishing_table_name);

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

	bool get_fishing_statistics(const unsigned int userid, const time_t start, const time_t end, const std::string& schema_name, const std::string& users_table_name, const std::string& fishing_table_name, const unsigned int skip, const unsigned int take, std::vector<fishing_entry>& vector) const
	{
        try
        {
            constexpr const char* get_statistics_command_template = "SELECT {5}.Id, {5}.Count, datediff(SECOND,{d '1970-01-01'}, {5}.CreatedAt) AS Date FROM {3}.{5} LEFT JOIN {3}.{4} ON {4}.Id = {5}.UserId WHERE {4}.Banned = 0 AND {5}.UserId = {0} AND {5}.Invalid = 0 AND {5}.CreatedAt BETWEEN dateadd(S, {1}, '1970-01-01') AND dateadd(S, {2}, '1970-01-01') ORDER BY Date DESC OFFSET {6} ROWS FETCH NEXT {7} ROWS ONLY;";

            std::string string = std::string(get_statistics_command_template);
            replace(string, "{0}", std::to_string(userid));
            replace(string, "{1}", std::to_string(start));
            replace(string, "{2}", std::to_string(end));
            replace(string, "{3}", schema_name);
            replace(string, "{4}", users_table_name);
            replace(string, "{5}", fishing_table_name);
            replace(string, "{6}", std::to_string(skip));
            replace(string, "{7}", std::to_string(take));

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

    bool get_fishing_records(const time_t start, const time_t end, const std::string& schema_name, const std::string& users_table_name, const std::string& fishing_table_name, const unsigned int skip, const unsigned int take, std::vector<fishing_record_entry>& vector) const
    {
        try
        {
            constexpr const char* get_statistics_command_template = "SELECT {3}.Login, SUM({4}.Count) AS Count FROM {2}.{4} LEFT JOIN {2}.{3} ON {3}.Id = {4}.UserId WHERE {3}.Banned = 0 AND {4}.Invalid = 0 AND {4}.CreatedAt BETWEEN dateadd(S, {0}, '1970-01-01') AND dateadd(S, {1}, '1970-01-01') GROUP BY {3}.Login ORDER BY Count DESC OFFSET {5} ROWS FETCH NEXT {6} ROWS ONLY;";

            std::string string = std::string(get_statistics_command_template);
            replace(string, "{0}", std::to_string(start));
            replace(string, "{1}", std::to_string(end));
            replace(string, "{2}", schema_name);
            replace(string, "{3}", users_table_name);
            replace(string, "{4}", fishing_table_name);
            replace(string, "{5}", std::to_string(skip));
            replace(string, "{6}", std::to_string(take));

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

    bool get_fishing_records(const unsigned int userid, const time_t start, const time_t end, const std::string& schema_name, const std::string& users_table_name, const std::string& fishing_table_name, std::vector<fishing_record_entry>& vector) const
    {
        try
        {
            constexpr const char* get_statistics_command_template = "SELECT {3}.Login, SUM({4}.Count) AS Count FROM {2}.{4} LEFT JOIN {2}.{3} ON {3}.Id = {4}.UserId WHERE {3}.Id = '{5}' AND {3}.Banned = 0 AND {4}.Invalid = 0 AND {4}.CreatedAt BETWEEN dateadd(S, {0}, '1970-01-01') AND dateadd(S, {1}, '1970-01-01') GROUP BY {3}.Login;";

            std::string string = std::string(get_statistics_command_template);
            replace(string, "{0}", std::to_string(start));
            replace(string, "{1}", std::to_string(end));
            replace(string, "{2}", schema_name);
            replace(string, "{3}", users_table_name);
            replace(string, "{4}", fishing_table_name);
            replace(string, "{5}", std::to_string(userid));

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

    bool set_ban_status(const unsigned int userid, const std::string& schema_name, const std::string& users_table_name) const
    {
        return set_ban_status(userid, true, schema_name, users_table_name);
    }

	bool set_ban_status(const unsigned int userid, const bool status, const std::string& schema_name, const std::string& users_table_name) const
	{
        try
        {
            constexpr const char* set_ban_status_command_template = "UPDATE {2}.{3} SET {3}.Banned = {1} WHERE {3}.Id = {0};";

            std::string string = std::string(set_ban_status_command_template);
            replace(string, "{0}", std::to_string(userid));
            replace(string, "{1}", std::to_string(status));
            replace(string, "{2}", schema_name);
            replace(string, "{3}", users_table_name);

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

    bool set_ban_status(const std::string& login, const std::string& schema_name, const std::string& users_table_name) const
    {
        return set_ban_status(login, true, schema_name, users_table_name);
    }

    bool set_ban_status(const std::string& login, const bool status, const std::string& schema_name, const std::string& users_table_name) const
    {
        try
        {
            constexpr const char* set_ban_status_command_template = "UPDATE {2}.{3} SET {3}.Banned = {1} WHERE {3}.Login = '{0}';";

            std::string string = std::string(set_ban_status_command_template);
            replace(string, "{0}", login);
            replace(string, "{1}", std::to_string(status));
            replace(string, "{2}", schema_name);
            replace(string, "{3}", users_table_name);

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

    bool set_ban_status(const user& user, const std::string& schema_name, const std::string& users_table_name) const
    {
        return set_ban_status(user, true, schema_name, users_table_name);
    }

    bool set_ban_status(const user& user, const bool status, const std::string& schema_name, const std::string& users_table_name) const
    {
        return set_ban_status(user.get_id(), status, schema_name, users_table_name);
    }
	
	bool rollback() const
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

    ~database()
    {
        connection_->Disconnect();
        delete this->connection_;
    }
};
