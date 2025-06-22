# include "../../includes/Config.hpp"
# include <iostream>
# include <fstream>
# include <stdexcept>

Config::Config(std::string input) : _errorCode(0)
{
    parseConfig(input);
}

void    Config::parseConfig(std::string input)
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

void    Config::addServer(Server server)
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