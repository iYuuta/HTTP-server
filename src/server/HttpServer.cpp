#include "../../includes/HttpServer.hpp"
#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>


HttpServer::HttpServer(Config& config) : _config(config)
{
}

void HttpServer::clean()
{
	for (std::vector<pollfd>::iterator it = _pollFds.begin(); it != _pollFds.end(); ++it)
		closeFd(it);
	_pollFds.clear();
	for (std::map<int, Client*>::iterator it = _clients.begin(); it != _clients.end(); ++it)
		delete it->second;
}


Client& HttpServer::getClient(const int& clientId)
{
	std::map<int, Client*>::iterator it = _clients.find(clientId);
	return *it->second;
}

void HttpServer::closeFd(std::vector<pollfd>::iterator it)
{
	delete _clients[it->fd];
	_clients.erase(it->fd);
}

void HttpServer::insertNewClient(const int& clientId, Server& server)
{
	if (isClientExists(clientId))
		return;
	Client *client = new Client(clientId, server, server.getErrorPages());
	if (!(_clients.insert(std::make_pair(clientId, client)).second))
		delete client;
}

bool HttpServer::isClientExists(const int& clientId)
{
	std::map<int, Client*>::iterator it = _clients.find(clientId);
	return (it != _clients.end());
}

Server& HttpServer::getServerByFd(const int& fd)
{
	std::vector<Server>& servers = _config.getServers();

	for (std::vector<Server>::iterator it = servers.begin(); it != servers.end(); ++it)
	{
		if (it->getFd() == fd)
			return *it;
	}
	throw std::runtime_error("Server not found");
}

void HttpServer::removePollFd(const pollfd& pfd)
{
	for (std::vector<pollfd>::iterator it = _pollFds.begin(); it != _pollFds.end(); ++it)
	{
		if (it->fd == pfd.fd)
		{
			closeFd(it);
			_pollFds.erase(it);
			return;
		}
	}
}

void HttpServer::newPollFd(const int& fd, const short& events)
{
	pollfd pollFd;
	pollFd.fd = fd;
	pollFd.events = events;
	pollFd.revents = 0; 
	_pollFds.push_back(pollFd);
}

void HttpServer::setupAll()
{
	std::vector<Server>& servers = _config.getServers();

	for (std::vector<Server>::iterator it = servers.begin(); it != servers.end(); ++it)
	{
		it->setup();
		newPollFd(it->getFd(), POLLIN);
	}
}

bool HttpServer::startAll()
{
	try
	{
		setupAll();
		listen();
		clean();
	}
	catch (const std::exception& e)
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
		const int rt = poll(_pollFds.data(), _pollFds.size(), 500);

		if (rt < 0)
		{
			std::cerr << "Error: poll failed, retying..." << std::endl;
			continue ;
		}
		if (rt == 0) {
			handleTimeOut();
			continue ;
		}

		for (size_t i = 0; i < _pollFds.size(); i++)
		{
			pollfd& pollFd = _pollFds[i];

			if (pollFd.revents & POLLIN)
			{
				if (i < _config.getServers().size())
					handleNewConnection(pollFd);
				else
					handleClientRequest(pollFd);
			}
			else if (pollFd.revents & POLLOUT)
				handleClientResponse(pollFd);
		}
	}
}

void HttpServer::handleTimeOut() {
	for (size_t i = _config.getServers().size(); i < _pollFds.size(); i++)
	{
		if (std::time(NULL) - _clients[_pollFds[i].fd]->getLastActivity() >= 30) {
			removePollFd(_pollFds[i]);
		}
		
	}
}

void HttpServer::handleNewConnection(pollfd& pollFd)
{
	Server& server = getServerByFd(pollFd.fd);
	const int clientFd = accept(pollFd.fd, NULL, NULL);

	if (clientFd < 0)
		return ;

	fcntl(clientFd, F_SETFL, O_NONBLOCK);

	insertNewClient(clientFd, server);

	newPollFd(clientFd, POLLIN);
}

void HttpServer::handleClientRequest(pollfd& pollFd)
{
	Client& client = getClient(pollFd.fd);

	if (!client.isRequestDone())
		client.parseRequest();

	if (client.clientFailed())
		return (removePollFd(pollFd));

	if (client.isRequestDone())
		pollFd.events = POLLOUT;
}

void HttpServer::handleClientResponse(pollfd& pollFd)
{
	Client& client = getClient(pollFd.fd);

	if (!client.isResponseBuilt())
		client.createResponse();
	else if (client.getResponseState() != DONE)
		client.sendResponse();

	if (client.clientFailed() || client.getResponseState() == DONE) 
		removePollFd(pollFd);
}