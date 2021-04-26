#include <iostream>

#include "apiserver.hpp"
#include "database.hpp"
#include "SQLAPI.h"
#include "nlohmann/json.hpp"

#include "config.hpp"

std::string host;
unsigned short int port;
std::string schema_name;
std::string users_table_name;
std::string fishing_table_name;
std::string peper;

database* database::current_;
std::string* fishing_record_entry::records_cache_;
uint64_t fishing_record_entry::records_cache_time_;

void generate_index_response(const httplib::Request& req, httplib::Response& res)
{
	if (req.has_header("User-Agent"))
	{
        res.set_content(req.get_header_value("User-Agent"), "text/plain");
	}

    res.status = 200;
}

void generate_register_response(const httplib::Request& req, httplib::Response& res)
{
    if (req.has_param("login") && req.has_param("password"))
    {
        connection connection;
        const std::string login = req.get_param_value("login");
        const std::string password = req.get_param_value("password");
        static const std::regex checker("^[0-9a-zA-Z]+$");
        std::smatch captures;

    	if (!std::regex_match(login, captures, checker) || !std::regex_match(password, captures, checker))
    	{
            res.status = 401;
            return;
    	}
    	
        if (connections::get_connection_by_login(login, connection))
        {
            if (connection.get_user().is_valid_password(password))
            {
                connections::remove_connection_by_login(login);
                return generate_register_response(req, res);
            }
        	
            res.status = 401;
        	return;
        }

        database* db = database::get_current_database();
        if (db)
        {
            user usr;
            if (req.has_param("eula"))
            {
	            if (req.get_param_value("eula") != "true")
	            {
                    res.status = 403;
                    return;
	            }

                usr = db->register_user(login, password, schema_name, users_table_name);
            }
            else
            {
                usr = db->get_user(login, schema_name, users_table_name);
            }

            if (!usr.is_valid_password(password) || usr.equals(user()))
            {
                res.status = 401;
                return;
            }
        	
            if (usr.is_banned())
            {
                res.status = 403;
                return;
            }
        	
            connection = connections::create_connection(usr);
            res.set_content(connection.get_apikey(), "text/plain");
            res.status = 200;
            return;
        }
    	
        res.status = 503;
        return;
    }

    res.status = 401;
}

void generate_statistics_response(const httplib::Request& req, httplib::Response& res)
{
    if (req.has_param("key"))
    {
        connection connection;
        if (connections::get_connection(req.get_param_value("key"), connection))
        {
            if (connection.get_user().is_banned())
            {
                res.status = 403;
                return;
            }

            database* db = database::get_current_database();
            if (db)
            {
                int start = 0;

            	if (req.has_param("start"))
            	{
                    try
                    {
                        start = std::stoi(req.get_param_value("start"));
                    }
                    catch (std::exception& _)
                    {
                    }
            	}

                if (start < 0)
                {
                    start = 0;
                }

                int end = 0x7FFFFFFF;

                if (req.has_param("end"))
                {
                    try
                    {
                        end = std::stoi(req.get_param_value("end"));
                    }
                    catch (std::exception& _)
                    {
                    }
                }

                if (end < 0)
                {
                    end = 0;
                }

                int skip = 0;

                if (req.has_param("skip"))
                {
                    try
                    {
                        skip = std::stoi(req.get_param_value("skip"));
                    }
                    catch (std::exception& _)
                    {
                    }
                }

                if (skip < 0)
                {
                    skip = 0;
                }

                int take = 0x7FFFFFFF;

                if (req.has_param("take"))
                {
                    try
                    {
                        take = std::stoi(req.get_param_value("take"));
                    }
                    catch (std::exception& _)
                    {
                    }
                }

                if (take == 0)
                {
                    res.set_content("[]", "application/json");
                    res.status = 200;
                    return;
                }

                if (take < 0)
                {
                    take = 0x7FFFFFFF;
                }

                std::vector<fishing_entry>* entries = new std::vector<fishing_entry>();
                if (db->get_fishing_statistics(connection.get_user().get_id(), start, end, schema_name, users_table_name, fishing_table_name, skip, take, *entries))
                {
                    json array = json::array();

                    for (const auto& entry : *entries)
                    {
                        json object = json::object();
                        object["Id"] = entry.get_id();
                        object["Count"] = entry.get_count();
                        object["Date"] = entry.get_create_date();
                        array.push_back(object);
                    }

                    res.set_content(array.dump(), "application/json");

                	delete entries;
                	
                    res.status = 200;
                    return;
                }

                delete entries;
            }

            res.status = 503;
            return;
        }

        res.status = 403;
        return;
    }

    res.status = 401;
}

struct fishing_anticheat
{
private:
    uint64_t last_;
    unsigned int cheat_requests_;

public:
    fishing_anticheat()
    {
        set_last_time();
        cheat_requests_ = 0;
    }
	
	uint64_t get_last_time() const
	{
        return this->last_;
	}

    uint64_t set_last_time()
    {
        return set_last_time(time_epoch());
    }

    uint64_t set_last_time(const uint64_t time)
	{
        this->last_ = time;
        return time;
	}

	unsigned int get_cheat_requests() const
	{
        return this->cheat_requests_;
	}

    unsigned int increase_cheat_requests()
	{
        return ++this->cheat_requests_;
	}
};

static std::unordered_map<unsigned int, fishing_anticheat>* fishing_anticheat_map = new std::unordered_map<unsigned int, fishing_anticheat>;

void generate_fishing_response(const httplib::Request& req, httplib::Response& res)
{
    if (req.has_param("key"))
    {
        connection connection;
        if (connections::get_connection(req.get_param_value("key"), connection))
        {
            if (connection.get_user().is_banned())
            {
                res.status = 403;
                return;
            }

            unsigned int id = connection.get_user().get_id();
            if (fishing_anticheat_map->find(id) == fishing_anticheat_map->end())
            {
                fishing_anticheat_map->insert({id, fishing_anticheat()});
            }
            else
            {
                fishing_anticheat& anticheat = fishing_anticheat_map->at(id);
            	
                const uint64_t previous = anticheat.get_last_time();
                const uint64_t current = anticheat.set_last_time();

            	if (current - previous < 1000)
            	{
                    if (anticheat.increase_cheat_requests() >= 10)
                    {
                        user& user = connection.get_user();
                        user.set_banned(true);

                        database* db = database::get_current_database();
                        if (db)
                        {
                            db->set_ban_status(user, schema_name, users_table_name);
                        }

                        connections::remove_connection(user);
                    	
                        res.status = 403;
                        return;
                    }
            	}
            }

            database* db = database::get_current_database();
        	if (db)
        	{
                if (db->register_fishing_transaction(connection.get_user().get_id(), schema_name, fishing_table_name));
                {
                    res.status = 200;
                	return;
                }
        	}

            res.status = 503;
        	return;
        }

        res.status = 403;
        return;
    }

    res.status = 401;
}

void generate_records_responce(const httplib::Request& req, httplib::Response& res)
{
    database* db = database::get_current_database();
    if (db)
    {
        int start = 0;

        if (req.has_param("start"))
        {
            try
            {
                start = std::stoi(req.get_param_value("start"));
            }
            catch (std::exception& _)
            {
            }
        }

        if (start < 0)
        {
            start = 0;
        }

        int end = 0x7FFFFFFF;

        if (req.has_param("end"))
        {
            try
            {
                end = std::stoi(req.get_param_value("end"));
            }
            catch (std::exception& _)
            {
            }
        }

        if (end < 0)
        {
            end = 0;
        }

        int skip = 0;

        if (req.has_param("skip"))
        {
            try
            {
                skip = std::stoi(req.get_param_value("skip"));
            }
            catch (std::exception& _)
            {
            }
        }

        if (skip < 0)
        {
            skip = 0;
        }

        int take = 10;

        if (req.has_param("take"))
        {
            try
            {
                take = std::stoi(req.get_param_value("take"));
            }
            catch (std::exception& _)
            {
            }
        }

        std::string apikey;
    	if (req.has_param("key"))
    	{
            apikey = req.get_param_value("key");

            connection connection;
    		if (connections::get_connection(apikey, connection))
    		{
                if (connection.get_user().is_banned())
                {
                    res.status = 403;
                    return;
                }

                const unsigned int id = connection.get_user().get_id();
    			
                std::vector<fishing_record_entry>* entries = new std::vector<fishing_record_entry>();
                if (db->get_fishing_records(id, start, end, schema_name, users_table_name, fishing_table_name, *entries))
                {
                    json array = json::array();

                    for (const auto& entry : *entries)
                    {
                        json object = json::object();
                        object["Id"] = entry.get_login();
                        object["Count"] = entry.get_count();
                        array.push_back(object);
                    }

                    res.set_content(array.dump(), "application/json");

                    delete entries;

                    res.status = 200;
                    return;
                }

                delete entries;
                res.status = 503;
    			return;
    		}

            res.status = 403;
            return;
    	}
    	
        if (take == 0)
        {
            res.set_content("[]", "application/json");
            res.status = 200;
            return;
        }

        if (take < 0)
        {
            take = 10;
        }

        const bool default_request = start == 0 && end == 0x7FFFFFFF && skip == 0 && take == 10;
    	
    	if (default_request && time_epoch() - fishing_record_entry::get_cache_time() < 300000)
    	{
            res.set_content(fishing_record_entry::get_cache(), "application/json");
            res.status = 200;
            return;
    	}

        std::vector<fishing_record_entry>* entries = new std::vector<fishing_record_entry>();
        if (db->get_fishing_records(start, end, schema_name, users_table_name, fishing_table_name, skip, take, *entries))
        {
            json array = json::array();

            for (const auto& entry : *entries)
            {
                json object = json::object();
                object["Id"] = entry.get_login();
                object["Count"] = entry.get_count();
                array.push_back(object);
            }

            const std::string dump = array.dump();

            if (default_request)
            {
                fishing_record_entry::set_cache(dump);
            }
        	
            res.set_content(dump, "application/json");

            delete entries;

            res.status = 200;
            return;
        }

        delete entries;
    }

    res.status = 503;
}

void generate_close_response(const httplib::Request& req, httplib::Response& res)
{
	if (req.has_param("key"))
	{
        if (connections::remove_connection(req.get_param_value("key")))
        {
            res.status = 200;
        	return;
        }
		
        res.status = 403;
		return;
	}
	
    res.status = 401;
}

int main()
{
    setlocale(LC_ALL, "russian");
	
    nlohmann::json config;
	
    if (!try_read_config(config))
    {
        try_save_config(config);
    }
	
    try_read_property<std::string>(config, "host", host, "localhost");
    try_read_property(config, "port", port, (unsigned short int) 3000);

    apiserver* server = new apiserver(host, port);
    server->get("/", generate_index_response);
    server->get("/register", generate_register_response);
    server->get("/statistics", generate_statistics_response);
    server->get("/fishing", generate_fishing_response);
    server->get("/records", generate_records_responce);
    server->get("/close", generate_close_response);

    
    try_read_property<std::string>(config, "schema_name", schema_name, "Fisher");
    
	try_read_property<std::string>(config, "users_table_name", users_table_name, "Users");
    
	try_read_property<std::string>(config, "fishing_table_name", fishing_table_name, "Fishing");
    
    try_read_property<std::string>(config, "peper", peper, "Peper");
    user::set_peper(peper);

    database* db = new database();
    database::set_current_database(db);
	
    try
    {
        std::string dbname;
        try_read_property<std::string>(config, "database", dbname, "Fisher");
        std::string dblogin;
        try_read_property<std::string>(config, "dblogin", dblogin, "sa");
        std::string dbpassword;
        try_read_property<std::string>(config, "dbpassword", dbpassword, "SQL");
		std::string patched;
        try_read_property<std::string>(config, "patched", patched, "true");
    	
        if (!db->connect_to_database(dbname.c_str(), dblogin.c_str(), dbpassword.c_str(), patched == "true"))
        {
            log("Can't connect to database", message_type::fatal);
            return 1;
        }

        log("Database connected");
    	
        if (!db->create_schema(schema_name))
        {
            log("Can't create schema", message_type::fatal);
            return 2;
        }

        if (!db->create_users_table(schema_name, users_table_name))
        {
            log("Can't create users table", message_type::fatal);
            return 3;
        }

        if (!db->create_fishing_table(schema_name, fishing_table_name))
        {
            log("Can't create fishing table", message_type::fatal);
            return 4;
        }

        log("Start server");
        server->listen();
        server->block();
        log("Stop server");

        log("Database disconnected");
    }
    catch (SAException& exception)
    {
        log_sqlapi_exception(exception);
    }

    delete server;
    delete db;

    system("pause");
}
