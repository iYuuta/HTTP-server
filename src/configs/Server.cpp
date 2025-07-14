#include "../../includes/Server.hpp"
#include "../../includes/utils.hpp"

Server::Server(): _port(-1)
{
}

void Server::setHost(const std::string &host)
{
    _host = host;
}

void Server::setPort(const int &port)
{
    _port = port;
}

const std::string   &Server::getHost() const
{
    return (_host);
}

const int &Server::getPort() const
{
    return (_port);
}

void Server::setMaxAllowedClientRequestSize(const Size &size)
{
    _maxAllowedClientRequestSize = size;
}