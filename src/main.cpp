# include "../includes/Config.hpp"
# include "../includes/HttpServer.hpp"

int main (int ac, char **av)
{
    // Config      conf("configs/test.conf");
    ac = 1;
    Config      conf(av[1]);
    HttpServer  server(conf);

    return (server.startAll());
}