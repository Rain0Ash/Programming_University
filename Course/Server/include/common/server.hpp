#pragma once

#include <string>
#include <thread>
#define CPPHTTPLIB_OPENSSL_SUPPORT
#include "cpp_httplib/httplib.h"

class server final
{
private:
	httplib::Server* server_;
	std::thread* listen_ = nullptr;
	std::string host_;
	unsigned short int port_;

	static void listen_internal(httplib::Server* server, const std::string* host, const unsigned short int* port);

public:
	static bool is_certificate_installed(const char* certificate, const char* privatekey);

	server() = delete;
	server(std::string host, unsigned short int port);
	server(const char* certificate, const char* privatekey, std::string host, unsigned short int port);
	void get(const char* pattern, const httplib::Server::Handler& handler) const;
	void post(const char* pattern, const httplib::Server::Handler& handler) const;
	void listen();
	void block() const;
	~server();
};
