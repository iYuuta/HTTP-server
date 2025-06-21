#include "../../includes/Server.hpp"

Server::Server()
{

}

Server::~Server()
{

}

void Server::setHost(std::string host)
{
    _host = host;
}

void Server::setPort(int port)
{
    _port = port;
}

const std::string &Server::getHost( void )
{
    return (_host);
}

int Server::getPort( void )
{
    return (_port);
}

