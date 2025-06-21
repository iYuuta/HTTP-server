# include "../../includes/Config.hpp"
# include <iostream>
# include <fstream>
# include <stdexcept>

Config::Config(std::string input)
{
    _errorCode = 0;
    parseConfig(input);
}

static bool operator>>(std::ifstream& in, Config conf)
{
    Server server;
    
    std::string line;

    std::getline(in, line);

    std::cout << line << std::endl;
    
	return (false);
}

void    Config::parseConfig(std::string input)
{
    std::ifstream conf(input);

    if (conf.fail())
    {
        _errorCode = 1;
        return ;
    }
    while (conf >> *this)
    {
        
    }
    conf.close();
}