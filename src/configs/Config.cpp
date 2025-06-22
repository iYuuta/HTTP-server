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