#pragma once

#define CPPHTTPLIB_OPENSSL_SUPPORT
#include "cpp_httplib/httplib.h"

class responses
{
public:
	responses() = delete;
	static void generate_index_response(const httplib::Request& req, httplib::Response& res);
	static void generate_register_response(const httplib::Request& req, httplib::Response& res);
	static void generate_statistics_response(const httplib::Request& req, httplib::Response& res);
	static void generate_fishing_response(const httplib::Request& req, httplib::Response& res);
	static void generate_records_response(const httplib::Request& req, httplib::Response& res);
	static void generate_close_response(const httplib::Request& req, httplib::Response& res);
};
