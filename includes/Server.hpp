# ifndef SERVER_HPP
# define SERVER_HPP

# include <iostream>
# include <vector>
# include "Size.hpp"
# include "Route.hpp"


#define PORT_MAX_VALUE 65535

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
        Server( );
        void                setHost(const std::string &host);
        void                setPort(const int &port);
        const std::string   &getHost() const;
        const int           &getPort() const;
        void                setMaxAllowedClientRequestSize(const Size &size);
};

bool parseHostAndPort( Server &server, std::vector<std::string> &content, size_t &i );
bool parseMaxRequestSize( Server &server, std::vector<std::string> &content, size_t &i );

#endif
