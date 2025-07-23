#include "../../includes/Server.hpp"
#include "../../includes/utils.hpp"

Server::Server(): _port(-1), _fd(-1)
{
}

void Server::setHost(const std::string &host)
{
    _host = host;
}

void Server::addErrorPage(const int &code, const std::string &page)
{
    _errorPages[code] = page;
}

void Server::setName(const std::string &name)
{
    _name = name;
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

void Server::setMaxAllowedClientRequestSize(size_t size)
{
    _maxAllowedClientRequestSize = size;
}

void Server::addLocation(const Location &location)
{
    _locations.push_back(location);
}
