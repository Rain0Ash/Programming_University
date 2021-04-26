#pragma once

#include <string>
#include <thread>
#include <cpp_httplib/httplib.h>

class apiserver final
{
private:
    httplib::Server* server_;
    std::thread* listen_;
    std::string host_;
    unsigned short int port_;

    static void listen_internal(httplib::Server* server, const std::string* host, const unsigned short int* port)
    {
        if (server)
        {
            server->listen(host->c_str(), *port);
        }
    }

public:
    apiserver(const std::string& host, unsigned short int port) : host_(host), port_(port)
    {
        server_ = new httplib::Server;
    }

    void get(const char* pattern, const httplib::Server::Handler& handler)
    {
        server_->Get(pattern, handler);
    }

    void post(const char* pattern, const httplib::Server::Handler& handler)
    {
        server_->Post(pattern, handler);
    }

    void listen()
    {
        if (listen_)
        {
            return;
        }

        listen_ = new std::thread(listen_internal, this->server_, &this->host_, &this->port_);
    }

    void block() const
    {
        if (listen_)
        {
            listen_->join();
        }
    }

    ~apiserver()
    {
        if (listen_)
        {
            server_->stop();
            listen_->join();
        }

        delete listen_;
        delete server_;
    }
};
