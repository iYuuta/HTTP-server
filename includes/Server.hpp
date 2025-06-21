# ifndef SERVER_HPP
# define SERVER_HPP

# include <iostream>
# include <vector>
# include "Size.hpp"
# include "Route.hpp"

class Server
{
    private:
        std::string                 _host;
        int                         _port;
        std::vector<std::string>    _names;
        std::string                 _errorPage;
        Size                        _maxAllowedClientRequestSize;
        std::vector<Route>          _routes;
    public:
        Server( void );
        void                setHost(std::string host);
        void                setPort(int port);
        const std::string   &getHost( void );
        int                 getPort( void );
};

#endif
