# include "../../includes/Config.hpp"
# include <iostream>
# include <fstream>
# include <stdexcept>
# include "../../includes/utils.hpp"

Config::Config(std::string input)
{
    _errorCode = 0;
    parseConfig(input);
}

static bool operator>>(std::ifstream& in, Config conf)
{
    Server server;
    std::string content;
    in >> content;
    std::vector<std::string> a = split(content);
    for (size_t i = 0; i < a.size(); ++i) {
        std::cout << a[i] << std::endl;
    }
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