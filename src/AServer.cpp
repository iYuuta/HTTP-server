#include "../includes/AServer.hpp"

AServer::AServer()
{

}

AServer::~AServer()
{

}

void    AServer::setHost(std::string host)
{
    _host = host;
}

void    AServer::setPort(int port)
{
    _port = port;
}

std::string AServer::getHost( void )
{
    return (_host);
}

int AServer::getPort( void )
{
    return (_port);
}

