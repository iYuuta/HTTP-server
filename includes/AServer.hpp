# pragma once

# include <iostream>

class AServer
{
    private:
        std::string     _host;
        int             _port;
    public:
        AServer( void );
        ~AServer();
        void            setHost(std::string host);
        void            setPort(int port);
        std::string     getHost( void );
        int             getPort( void );
};

