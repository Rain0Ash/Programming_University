// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "server.hpp"

#include "utils.hpp"

void server::listen_internal(httplib::Server* server, const std::string* host, const unsigned short* port)
{
	if (server)
	{
		server->listen(host->c_str(), *port);
	}
}

bool server::is_certificate_installed(const char* certificate, const char* privatekey)
{
	std::ifstream cert(certificate);
	std::ifstream key(privatekey);

	const bool exists = !utils::is_empty(cert) && !utils::is_empty(key);

	cert.close();
	key.close();

	return exists;
}

server::server(std::string host, const unsigned short port) : host_(std::move(host)), port_(port)
{
	server_ = new httplib::Server();
}

server::server(const char* certificate, const char* privatekey, std::string host, const unsigned short port) : host_(std::move(host)), port_(port)
{
	server_ = new httplib::SSLServer(certificate, privatekey);
}

void server::get(const char* pattern, const httplib::Server::Handler& handler) const
{
	server_->Get(pattern, handler);
}

void server::post(const char* pattern, const httplib::Server::Handler& handler) const
{
	server_->Post(pattern, handler);
}

void server::listen()
{
	if (listen_)
	{
		return;
	}

	listen_ = new std::thread(listen_internal, this->server_, &this->host_, &this->port_);
}

void server::block() const
{
	try
	{
		if (listen_)
		{
			listen_->join();
		}
	}
	catch (std::exception&)
	{
	}
}

server::~server()
{
	if (listen_)
	{
		try
		{
			server_->stop();
			listen_->join();
		}
		catch (...)
		{
		}
	}

	delete listen_;
	delete server_;
}
