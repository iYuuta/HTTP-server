# ifndef SERVER_HPP
# define SERVER_HPP

# include <iostream>
# include <vector>

# include "Location.hpp"
# include "Size.hpp"

#define PORT_MAX_VALUE 65535

class Server
{
    private:
        std::string                              _host;
        int                                      _port;
        std::string                              _name;
        std::vector<std::pair<int, std::string> > _errorPages;
        Size                                     _maxAllowedClientRequestSize;
        std::vector<Location>                    _locations;
    public:
        Server( );
        void                setHost(const std::string &host);
        void                setName(const std::string &name);
        void                addErrorPage(const int &code, const std::string &page);
        void                setPort(const int &port);
        const std::string   &getHost() const;
        const int           &getPort() const;
        void                setMaxAllowedClientRequestSize(const Size &size);
        void                addLocation(const Location &location);
};

bool parseHostAndPort( Server &server, std::vector<std::string> &content, size_t &i );
bool parseMaxRequestSize( Server &server, std::vector<std::string> &content, size_t &i );

#endif
