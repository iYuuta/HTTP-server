#include "../../includes/HttpServer.hpp"
#include <iostream>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/select.h>
#include <arpa/inet.h>

HttpServer::HttpServer(Config& config) : _config(config)
{
}

void HttpServer::setup(Server& server)
{
	sockaddr_in address;

	address.sin_family = AF_INET;
	address.sin_port = htons(server.getPort());
	address.sin_addr.s_addr = INADDR_ANY;

	const int fd = socket(AF_INET, SOCK_STREAM, 0);
	if (fd < 0)
		throw std::runtime_error("Socket creation failed");

	server.setFd(fd);
	if (bind(fd, (sockaddr*)&address, sizeof(address)) < 0)
		throw std::runtime_error("Socket bind failed");

	if (listen(fd, SOMAXCONN) < 0)
		throw std::runtime_error("Socket listen failed");

	pollfd pollFd;
	pollFd.fd = fd;
	pollFd.events = POLLIN;
	_pollFds.push_back(pollFd);
}

void HttpServer::setupAll()
{
	std::vector<Server>& servers = _config.getServers();

	for (std::vector<Server>::iterator it = servers.begin(); it != servers.end(); ++it)
	{
		setup(*it);
	}
}


bool HttpServer::startAll()
{
	try
	{
		setupAll();
		listenAll();
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

void HttpServer::listenAll()
{
	while (true)
	{
		const int rt = poll(_pollFds.data(), _pollFds.size(), -1);

		if (rt < 0)
			throw std::runtime_error("poll failed");
		if (rt == 0)
			throw std::runtime_error("TIMEOUT");
		for (std::vector<pollfd>::iterator it = _pollFds.begin(); it != _pollFds.end(); ++it)
		{
			
		}
	}
}

void HttpServer::clean()
{
	for (std::vector<pollfd>::iterator it = _pollFds.begin(); it != _pollFds.end(); ++it)
	{
		close(it->fd);
	}
}
