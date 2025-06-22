#include "../../includes/Server.hpp"
#include "../../includes/utils.hpp"

Server::Server()
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

static bool  isValidPort(const std::string &str)
{
    if (str.empty() || str.size() > 5)
        return (false);
    if (str[0] == '0')
        return (!str[1]);
    for (size_t i = 0; i < str.size(); i++)
    {
        if (!std::isdigit(str[i]))
            return (false);
    }
    int port = std::atoi(str.c_str());
    return (port <= PORT_MAX_VALUE);
}


bool Server::parseHostAndPort( std::vector<std::string> &content, size_t &i )
{
    std::vector<std::string> list = split(content[i], ':');
    if (list.size() != 2)  
        return (std::cerr << "A valid listen value should look something like 127.0.0.1:8080" << std::endl, false);
    std::string &portStr = list[1];
    if (!isValidPort(portStr))
        return (std::cerr << portStr << ": is not a valid port!" << std::endl, false);
    std::cout << portStr << "\n";
    return (false);
}