# include "../../includes/Config.hpp"
# include <iostream>
# include <fstream>

Config::Config(const std::string &input) : _errorCode(0)
{
    parseConfig(input);
}

void    Config::parseConfig(const std::string &input)
{
    std::ifstream conf(input);

    if (conf.fail())
    {
        _errorCode = 1;
        return ;
    }
    if (!parseServers(conf, *this))
        _errorCode = 2;
    conf.close();
}

void    Config::addServer(const Server &server)
{
    _servers.push_back(server);
}

void    Config::test()
{
    for (size_t i = 0; i < _servers.size(); i++)
    {
        std::cout << _servers[i].getHost() << std::endl;
    }
}