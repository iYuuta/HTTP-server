# include "../includes/Config.hpp"
# include "../includes/HttpServer.hpp"

int main ()
{
    Config      conf("configs/test.conf");
    HttpServer  server(conf);

    return (server.startAll());
}