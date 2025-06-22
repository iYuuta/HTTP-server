# ifndef CONFIG_HPP
# define CONFIG_HPP

# include "Server.hpp"
# include <vector>


class Config
{
    public:
        Config(std::string input);
    private:
        short               _errorCode;
        std::vector<Server> _servers;
        void    parseConfig(std::string input);
};

bool parseServers(std::ifstream& in, Config conf);


#endif