# include "../includes/Config.hpp"
# include "../includes/HttpServer.hpp"

int main (int ac, char **av)
{
    if (ac != 2)
        return (std::cout << "usage: ./webserv config_file_path" << std::endl, 1);

    Config      conf(av[1]);

    if (conf.fail())
        return (conf.getErrorCode());

    HttpServer  server(conf);
    return (server.startAll());
}