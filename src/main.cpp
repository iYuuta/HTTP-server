# include "../includes/Config.hpp"
# include "../includes/HttpServer.hpp"

int main (int ac, char **av)
{
    std::string path("configs/test.conf");

    if (ac == 2)
        path = av[1];
    else if (ac != 1)
        return (std::cerr << "usage: ./webserv config_file_path" << std::endl, 1);

    Config conf(path);
    if (conf.fail())
        return (conf.getErrorCode());
    signal(SIGPIPE, SIG_IGN);
    HttpServer  server(conf);
    return (server.startAll() ? 0 : 69);
}