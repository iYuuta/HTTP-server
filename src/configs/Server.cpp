#include "../../includes/Server.hpp"
#include "../../includes/utils.hpp"

Server::Server(): _port(-1)
{
}

void Server::SetHost(const std::string &host)
{
    _host = host;
}

void Server::AddErrorPage(const int &code, const std::string &page)
{
    _errorPages.push_back(std::make_pair(code, page));
}

void Server::SetName(const std::string &name)
{
    _name = name;
}

void Server::SetPort(const int &port)
{
    _port = port;
}

const std::string   &Server::GetHost() const
{
    return (_host);
}

const int &Server::GetPort() const
{
    return (_port);
}

void Server::SetMaxAllowedClientRequestSize(size_t size)
{
    _maxAllowedClientRequestSize = size;
}

void Server::AddLocation(const Location &location)
{
    _locations.push_back(location);
}
