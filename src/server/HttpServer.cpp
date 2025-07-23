#include "../../includes/HttpServer.hpp"


HttpServer::HttpServer(Config& config) : _config(config)
{
}


static void start(Server &server)
{
	std::cout << server.getHost() << std::endl;
}

void HttpServer::startAll() const
{
	_config.forEachServer(start);
}
