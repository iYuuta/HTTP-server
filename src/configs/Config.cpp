# include "../../includes/Config.hpp"
# include <iostream>
# include <fstream>

Config::Config(const std::string &input) : _errorCode(0)
{
    parseConfig(input);
}

void    Config::parseConfig(const std::string &input)
{
    std::ifstream conf(input.c_str());

    if (conf.fail())
    {
        _errorCode = 1;
        return ;
    }
    if (!parseServers(conf, *this))
        _errorCode = 2;
    conf.close();
}

void    Config::addServer(Server &server)
{
    _servers.push_back(server);
    server.tempserver();
}

void    Config::test()
{
//     for (auto & _server : _servers)
//     {
//         std::cout << _server.GetHost() << std::endl;
//     }
}