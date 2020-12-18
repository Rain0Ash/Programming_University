#pragma once

#include "config.h"
#include "cpp_httplib/httplib.h"
#include "nlohmann/json.hpp"

using json = nlohmann::json;

enum class voice_type
{
	silent,
	voice
};

static std::map<voice_type, std::string> voice_type_to_string
{
	{voice_type::silent, "silent"},
	{voice_type::voice, "voice"}
};

static std::map<std::string, voice_type> voice_type_from_string
{
	{"silent", voice_type::silent},
	{"voice", voice_type::voice}
};

enum class skill_type
{
	basic,
	help
};

static std::map<skill_type, std::string> skill_type_to_string
{
	{skill_type::basic, "default"},
	{skill_type::help, "help"}
};

static std::map<std::string, skill_type> skill_type_from_string
{
	{"default", skill_type::basic},
	{"help", skill_type::help}
};

static json sessions = json::array();

const json help_button =
{
	{
		{"title", u8"������"},
		{"hide", true}
	},
};

const json voice_button =
{
	{"title", u8"��������"},
	{"hide", true}
};

const json silent_button =
{
	{"title", u8"�������"},
	{"hide", true}
};

const json help_buttons =
{
	{
		{"title", u8"�������"},
		{"hide", true}
	},
	{
		{"title", u8"��������"},
		{"hide", true}
	},
	{
		{"title", u8"������"},
		{"hide", true}
	},
	{
		{"title", u8"�������"},
		{"hide", true}
	},
	{
		{"title", u8"������� ���������"},
		{"hide", true}
	},
	{
		{"title", u8"����� �� ������"},
		{"hide", true}
	},
};

struct message
{
	std::string text;
	std::string tts;

	message() = default;

	message(const std::string& text, const char* tts)
	{
		this->text = text;
		this->tts = tts;
	}

	message(const std::string& text, const std::string& tts)
	{
		this->text = text;
		this->tts = tts;
	}

	message(const std::string& text, const bool& voice = true)
	{
		this->text = text;

		if (voice)
		{
			this->tts = text;
		}
	}
};

static std::map<std::string, message> help_messages =
{
	{u8"�������", message(u8"� ��������� ���������� ���������.")},
	{u8"��������", message(u8"� ����� ���������� ���������")},
	{u8"������", message(u8"� �������� ��� �������� � ������������.")},
	{u8"�������",message(u8"����� �������� ���-��, ������� \"�������� � ������� {�������} {����} ������\".\n����� ������� ���-��, ������� \"������� �� ������� {�������}\".")},
	{u8"�����", message(u8"��������� ��������� ������� � �������.")},
	{u8"������� ���������", message(u8"�������� ������� � ������� ������ ������� ����������� ��������.")},
	{u8"����� �� ������", message(u8"��������� ���")}
};

bool is_new_session(const json& request)
{
	return request["session"]["new"].get<bool>();
}

bool is_help_mode(const json& session)
{
	return session["skill_type"].get<std::string>() == skill_type_to_string[skill_type::help];
}

bool is_silent_mode(const json& session)
{
	return session["voice_type"].get<std::string>() == voice_type_to_string[voice_type::silent];
}

json generate_yandex_response(const message& message, const json& buttons, const json& session = nullptr,
                         const bool& end = false)
{
	json response =
	{
		{
			"response",
			{
				{"buttons", buttons},
				{"end_session", end}
			}
		},
		{"version", "1.0"}
	};

	if (!message.text.empty())
	{
		response["response"]["text"] = message.text;
	}

	if (session != nullptr)
	{
		if (is_silent_mode(session))
		{
			response["response"]["buttons"].push_back(voice_button);
			response["response"]["tts"] = std::string("sil <[500]>"); //�������� ���������
			return response;
		}

		if (!message.tts.empty())
		{
			response["response"]["tts"] = message.tts;
		}

		response["response"]["buttons"].push_back(silent_button);
		return response;
	}

	return response;
}

json generate_help_yandex_responce(const std::string& command, json& session)
{
	if (command == u8"����� �� ������")
	{
		session["skill_type"] = skill_type_to_string[skill_type::basic];
		return generate_yandex_response(help_messages[command], help_button, session);
	}
	
	return generate_yandex_response(
		help_messages.find(command) != help_messages.end()
			? message(help_messages[command].text + std::string(u8"\n� ��� ��� ����������?"))
			: message(u8"� ���� � �� ���� ��� ������.", u8"� +���� � �� ���+� ��� ���+���."),
		help_buttons, session);
}

void set_responce(httplib::Response& res, const message& message, const json& buttons, const json& session, const bool& end = false)
{
	res.set_content(generate_yandex_response(message, buttons, session, end).dump(4), "application/json; charset=UTF-8");
}

void on_silent_command(httplib::Response& res, json& session)
{
	session["voice_type"] = voice_type_to_string[voice_type::silent];
	set_responce(res, message(u8"�����, �����", false), help_button, session);
}

void on_voice_command(httplib::Response& res, json& session)
{
	session["voice_type"] = voice_type_to_string[voice_type::voice];
	set_responce(res, message(u8"������.", u8"�����+�."), help_button, session);
}

void on_help_command(httplib::Response& res, json& session)
{
	const static std::string text =
		std::string(u8"��������/������� - ��������/��������� ��������� �����.\n") +
		std::string(u8"�������. ��������� ����� ������ �������, � ��� �� �� ���������.\n") +
		std::string(u8"������. ������������ � ������������ ����� ������.\n") +
		std::string(u8"��������� �������. ������� ������� � ��������� ������ � ������� ������.\n") +
		std::string(u8"�����. ������� ����� ������� � �������� � ���.\n") +
		std::string(u8"� ��� ���������� ���������?");

	session["skill_type"] = skill_type_to_string[skill_type::help];
	set_responce(res, message(text), help_buttons, session);
}

void on_clear_cart_command(httplib::Response& res, json& session)
{
	if (session["cart"].empty())
	{
		set_responce(res, message(u8"������� � ��� �����.", u8"����+��� � ��� �����."), help_button, session);
		return;
	}

	session.erase("cart");
	session["cart"] = json::array();
	set_responce(res, message(u8"������� �����.", u8"����+��� �����."), help_button, session);
}

void on_what_in_cart_command(httplib::Response& res, json& session)
{
	if (session["cart"].empty())
	{
		set_responce(res, message(u8"���� ������� �����.", u8"���� ������� �����."), help_button, session);
		return;
	}

	std::string text = u8"�� ������ ������ � ����� �������:";
	for (auto& item : session["cart"])
	{
		const int price = item["price"].get<int>();

		text += "\n" + item["item"].get<std::string>() + u8" ����� " + std::to_string(price);

		if (price >= 10 && price <= 20 || price % 10 == 0)
		{
			text.append(u8" ������,");
		}
		else if (price % 10 == 1)
		{
			text.append(u8" �����,");
		}
		else if (price % 10 > 0 && price % 10 < 5)
		{
			text.append(u8" �����,");
		}
		else
		{
			text.append(u8" ������,");
		}
	}

	text[text.size() - 1] = '.';

	set_responce(res, message(text), help_button, session);
}

bool try_send_post_on_purchase_completed_command(const std::string& id, json& session)
{
	try
	{
		const json cart =
		{
			{"user_id", id},
			{"cart", session["cart"]}
		};

		json cfg;
		try_read_config(cfg);

		for (std::string hook : cfg["webhooks"])
		{
			trim(hook);
			replace(hook, "https://", "http://");
			
			if (hook.find("http://") != 0)
			{
				continue;
			}

			if (hook.find('/', 7) == std::string::npos)
			{
				hook.push_back('/');
			}
			
			const size_t index = hook.find('/', static_cast<std::string>("http://").size());

			if (index == std::string::npos)
			{
				log(std::string(u8"��� �������� ������� ���� ���������� ������������ �������: ") + hook, message_type::warning);
				continue;
			}
			
			httplib::Client client(hook.substr(0, index).c_str());
			client.Post(hook.substr(index, std::string::npos).c_str(), cart.dump(4), "application/json; charset=UTF-8");
			log(std::string(u8"������������ ������ �� ") + hook, message_type::info);
		}

		return true;
	}
	catch (const std::exception& ex)
	{
		log(std::string(u8"��������� ����������� ������ �� ����� �������� ��������: ") + ex.what(), message_type::critical);
		return false;
	}
}

void on_purchase_completed_command(httplib::Response& res, json& session, std::string& id)
{
	if (session["cart"].empty())
	{
		set_responce(res, message(u8"���� ������� �����.", u8"���� ������� �����."), help_button, session);
		return;
	}

	if (!try_send_post_on_purchase_completed_command(id, session))
	{
		set_responce(res, message(u8"��������� ����������� ������!\n���������� ��� ��� ��� ��������� �� �����������."), help_button, session);
		return;
	}

	session.erase("cart");
	session["cart"] = json::array();

	set_responce(res, message(u8"�������� ���!", u8"�����+��� ��+�!"), help_button, session, true);
}

void on_price_command(httplib::Response& res, json& session, const json& request)
{
	size_t size = request["request"]["nlu"]["tokens"].size();
	size_t fullprice = 0;

	for (auto& item : session["cart"])
	{
		fullprice += item["price"].get<size_t>();
	}

	if (fullprice <= 0)
	{
		set_responce(res, message(u8"� ��� � ������� ��� �������.", u8"� ��� � ����+��� ��� ���+����."), help_button,session);
		return;
	}

	std::string text = u8"� ����� ����� � ��� ������� �� " + std::to_string(fullprice);
	std::string tts = u8"� +����� �+���� � ��� ���+���� �� " + std::to_string(fullprice);

	if (fullprice >= 10 && fullprice <= 20 || fullprice % 10 == 0)
	{
		text.append(u8" ������.");
		tts.append(u8" ����+��.");
	}
	else if (fullprice % 10 == 1)
	{
		text.append(u8" �����.");
		tts.append(u8" �����.");
	}
	else if (fullprice % 10 > 0 && fullprice % 10 < 5)
	{
		text.append(u8" �����.");
		tts.append(u8" ����+�.");
	}
	else
	{
		text.append(u8" ������.");
		tts.append(u8" ����+��.");
	}

	set_responce(res, message(text, tts), help_button, session);
}

void on_add_to_cart_command(httplib::Response& res, json& session, const json& request)
{
	const size_t size = request["request"]["nlu"]["tokens"].size();
	std::string item_name;
	size_t price = 0;
	int index = 0;
	bool ns_index = false;

	for (auto item : request["request"]["nlu"]["entities"])
	{
		if (item["type"].get<std::string>() == "YANDEX.NUMBER")
		{
			index = item["tokens"]["start"];
			const auto val = item["value"].get<long long int>();

			if (val <= 0)
			{
				set_responce(res, message(u8"���� �� ����� ���� ������������� ��� ������ ����.", u8"���+� �� �+���� �+��� �����+�������� ��� ������ ����."), help_button, session);
				return;
			}
			
			price = val;
			ns_index = true;
			break;
		}
	}
	
	if (size == 3)
	{
		set_responce(res, message(u8"�� ������� ��� �������� � �������."), help_button, session);
		return;
	}

	if (!ns_index)
	{
		set_responce(res, message(u8"�� ������� ���� ������"), help_button, session);
		return;
	}

	if (index == 3)
	{
		set_responce(res, message(u8"�� ������� �������� ������"), help_button, session);
		return;
	}

	for (int i = 3; i < index; ++i)
	{
		item_name.append(request["request"]["nlu"]["tokens"][i].get<std::string>());

		if (i < index - 1)
		{
			item_name.append(" ");
		}
	}

	const json item =
	{
		{"item", item_name},
		{"price", price}
	};
	
	session["cart"].push_back(item);

	set_responce(res, message(u8"��.", u8"��+��."), help_button, session);
}

void on_remove_from_cart_command(httplib::Response& res, json& session, const json& request)
{
	const size_t size = request["request"]["nlu"]["tokens"].size();

	std::string item_name;

	for (size_t i = 3; i < size; ++i)
	{
		item_name.append(request["request"]["nlu"]["tokens"][i].get<std::string>());
		item_name.append(" ");
	}

	if (item_name.empty())
	{
		set_responce(res, message(u8"�� ������� ��� ������ �� �������"), help_button, session);
		return;
	}

	item_name.pop_back();

	for (size_t i = 0; i < session.size(); i++)
	{
		auto& item = session[i];
		if (item["item"].get<std::string>() == item_name)
		{
			session["cart"].erase(session["cart"].begin() + i);
			set_responce(res, message(u8"�������.", u8"����+���."), help_button, session);
			return;
		}
	}

	log(u8"������� �� ��� ������", message_type::info);
	set_responce(res, message(u8"������ �������� � ����� ������� ����.", u8"���+��� �������� � �+���� ����+��� �+���."), help_button, session);
}

inline void on_default_command(httplib::Response& res, json& session)
{
	set_responce(res, message(u8"� �� ���� ����� �������.", u8"� �� ��+�� ���+�� ���+����."), help_button, session);
}

json* find_session(const std::string& id)
{
	for (auto& session : sessions)
	{
		if (session["user_id"].get<std::string>() == id)
		{
			return &session;
		}
	}

	return nullptr;
}

void on_new_session(httplib::Response& res, const std::string& id)
{
	json* session_ptr = find_session(id);

	if (session_ptr == nullptr)
	{
		const json session =
		{
			{"user_id", id},
			{"skill_type", skill_type_to_string[skill_type::basic]},
			{"voice_type", voice_type_to_string[voice_type::voice]},
			{"cart", json::array()}
		};

		sessions.push_back(session);
		set_responce(res, message(u8"������������! � ������ ��� � ���������.", u8"���+���������! � �����+� ��� � ���+������."), help_button, session);
		return;
	}

	json& session = *session_ptr;
	session["skill_type"] = skill_type_to_string[skill_type::basic];
	session["voice_type"] = voice_type_to_string[voice_type::voice];
	set_responce(res, message(u8"������������! � ������ ��� � ���������.", u8"���+���������! � �����+� ��� � ���+������."), help_button, session);
}

void alice_yandex_hook(const httplib::Request& req, httplib::Response& res)
{
	json request = json::parse(req.body);

	std::string id = request["session"]["application"]["application_id"].get<std::string>();
	json response;
	json* session_ptr = find_session(id);

	if (is_new_session(request))
	{
		on_new_session(res, id);
		return;
	}

	if (session_ptr == nullptr)
	{
		set_responce(res, message(u8"��������, ��������� ������. �������� ������ ���� ��������.", u8"�����+���, ��������+� ��+����. �������� ������ ���� ��������."), help_button,
		             nullptr, true);
		return;
	}

	json& session = *session_ptr;

	const std::string command = request["request"]["command"].get<std::string>();

	if (is_help_mode(session))
	{
		res.set_content(generate_help_yandex_responce(command, session).dump(4), "application/json; charset=UTF-8");
		return;
	}

	if (command == u8"�������")
	{
		on_silent_command(res, session);
	}
	else if (command == u8"��������")
	{
		on_voice_command(res, session);
	}
	else if (command == u8"������")
	{
		on_help_command(res, session);
	}
	else if (command == u8"�������� �������" || command == u8"������ �������")
	{
		on_clear_cart_command(res, session);
	}
	else if (command == u8"��� � �������" || command == u8"�������")
	{
		on_what_in_cart_command(res, session);
	}
	else if (command == u8"������� ���������" || command == u8"���������")
	{
		on_purchase_completed_command(res, session, id);
	}
	else if (command == u8"�����" || command == u8"����")
	{
		on_price_command(res, session, request);
	}
	else if (command.find(u8"�������� � �������") == 0 || command.find(u8"������ � �������") == 0)
	{
		on_add_to_cart_command(res, session, request);
	}
	else if (command.find(u8"������� �� �������") == 0 || command.find(u8"����� �� �������") == 0 || command.
		find(u8"������ �� �������") == 0 || command.find(u8"����� �� �������") == 0)
	{
		on_remove_from_cart_command(res, session, request);
	}
	else
	{
		on_default_command(res, session);
	}
}
