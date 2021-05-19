// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "responses.hpp"

#include "nlohmann/json.hpp"
#include "anticheat.hpp"
#include "connections.hpp"
#include "database.hpp"

void responses::generate_index_response(const httplib::Request& req, httplib::Response& res)
{
	if (req.has_header("User-Agent"))
	{
		res.set_content(req.get_header_value("User-Agent"), "text/plain");
	}

	res.status = 200;
}

void responses::generate_register_response(const httplib::Request& req, httplib::Response& res)
{
	if (req.has_param("login") && req.has_param("password"))
	{
		connection connection;
		const std::string login = req.get_param_value("login");
		const std::string password = req.get_param_value("password");
		static const std::regex checker("^[0-9a-zA-Z]{1,64}$");
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

				usr = db->register_user(login, password);
			}
			else
			{
				usr = db->get_user(login);
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

void responses::generate_statistics_response(const httplib::Request& req, httplib::Response& res)
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
					catch (...)
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
					catch (...)
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
					catch (...)
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
					catch (...)
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
				if (db->get_fishing_statistics(connection.get_user().get_id(), start, end, skip, take, *entries))
				{
					nlohmann::json array = nlohmann::json::array();

					for (const auto& entry : *entries)
					{
						nlohmann::json object = nlohmann::json::object();
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

void responses::generate_fishing_response(const httplib::Request& req, httplib::Response& res)
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
			if (anticheat::container().find(id) == anticheat::container().end())
			{
				anticheat::container().insert({id, anticheat()});
			}
			else
			{
				anticheat& anticheat = anticheat::container().at(id);

				const uint64_t previous = anticheat.get_last_time();
				const uint64_t current = anticheat.set_last_time();

				if (current - previous < anticheat::get_cheat_request_timeout())
				{
					if (anticheat.increase_cheat_requests() > anticheat::get_max_cheat_requests())
					{
						user& user = connection.get_user();
						user.set_banned(true);

						database* db = database::get_current_database();
						if (db)
						{
							db->set_ban_status(user);
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
				if (db->register_fishing_transaction(connection.get_user().get_id()))
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

void responses::generate_records_response(const httplib::Request& req, httplib::Response& res)
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
			catch (...)
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
			catch (...)
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
			catch (...)
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
			catch (...)
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
				if (db->get_fishing_records(id, start, end, *entries))
				{
					nlohmann::json array = nlohmann::json::array();

					for (const auto& entry : *entries)
					{
						nlohmann::json object = nlohmann::json::object();
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

		if (default_request && utils::time_epoch() - fishing_record_entry::get_cache_time() < 300000)
		{
			res.set_content(fishing_record_entry::get_cache(), "application/json");
			res.status = 200;
			return;
		}

		std::vector<fishing_record_entry>* entries = new std::vector<fishing_record_entry>();
		if (db->get_fishing_records(start, end, skip, take, *entries))
		{
			nlohmann::json array = nlohmann::json::array();

			for (const auto& entry : *entries)
			{
				nlohmann::json object = nlohmann::json::object();
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

void responses::generate_close_response(const httplib::Request& req, httplib::Response& res)
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
