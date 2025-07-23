#include "../../includes/HttpServer.hpp"
#include <iostream>
#include <cstring>
#include <cerrno>
#include <cstdlib>
#include <cstdio>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <sys/select.h>
#include <arpa/inet.h>

HttpServer::HttpServer(Config& config) : _config(config)
{
}

static void setup(Server& server)
{
	sockaddr_in address;

	address.sin_family = AF_INET;
	address.sin_port = htons(server.getPort());
	address.sin_addr.s_addr = INADDR_ANY;

	const int listen_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (listen_fd < 0)
		throw std::runtime_error("Socket creation failed");

	if (bind(listen_fd, (sockaddr*)&address, sizeof(address)) < 0)
		throw std::runtime_error("Socket bind failed");

	if (listen(listen_fd, 10) < 0)
		throw std::runtime_error("Socket listen failed");
}

bool HttpServer::startAll()
{
	try
	{
		_config.forEachServer(setup);
		listen();
		clean();
	}
	catch (std::exception& e)
	{
		std::cerr << e.what() << std::endl;
		clean();
		return (false);
	}
	return (true);
}

void HttpServer::listen()
{
	while (true)
	{
	}
}

void HttpServer::clean()
{
}
