# ifndef CONFIG_HPP
# define CONFIG_HPP

# include "Server.hpp"
# include <vector>

class Config
{
    private:
        std::vector<Server> _servers;
};

#endif