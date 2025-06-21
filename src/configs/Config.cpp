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

static int getServerTagEnd(std::vector<std::string> content, size_t start)
{
    int     end = start + 1;
    int     openings = 0;
    size_t  size = content.size();

    while (end < size)
    {
        if (content[end] == "{")
            openings++;
        else if (content[end] == "}")
            openings--;
        if (openings == 0)
            return (end);
        end++;
    }
    return (-1);
}

static bool parseServer(std::vector<std::string> content, size_t &i)
{
    int end = getServerTagEnd(content, i);
    if (end == -1)
        return (false);
    std::cout << "STAT: " << content[i] << "| end: " << content[end] << std::endl;
    i += end;
    return (true);
}

static bool parseServers(std::ifstream& in, Config conf)
{
    std::string content;
    in >> content;
    std::vector<std::string> a = split(content);

    for (size_t i = 0; i < a.size(); i++) {
        if (a[i] == "server")
        {
            if (!parseServer(a, i))
                return (false);
        }
        else
	        return (std::cerr << "Uknown key at line: " << a[i] << std::endl, false);
    }
	return (true);
}

void    Config::parseConfig(std::string input)
{
    std::ifstream conf(input);

    if (conf.fail())
    {
        _errorCode = 1;
        return ;
    }
    parseServers(conf, *this);
    conf.close();
}