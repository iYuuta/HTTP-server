# ifndef CONFIG_HPP
# define CONFIG_HPP

# include "Server.hpp"
# include <vector>

class Config
{
    public:
        Config(const std::string &input);
        void    addServer(const Server &server);
        void    test();
    private:
        short               _errorCode;
        std::vector<Server> _servers;
        void    parseConfig(const std::string &input);
};

bool parseServers(std::ifstream& in, Config &conf);


#endif