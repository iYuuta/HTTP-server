# pragma once

# include <iostream>

class Server
{
    private:
        std::string     _host;
        int             _port;
    public:
        Server( void );
        ~Server();
        void                setHost(std::string host);
        void                setPort(int port);
        const std::string   &getHost( void );
        int                 getPort( void );
};

