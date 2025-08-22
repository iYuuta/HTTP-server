#include "../../includes/Server.hpp"

#include <netdb.h>
#include <string.h>

Server::Server(): _fd(-1), _port(-1), _maxAllowedClientRequestSize(-1)
{
}

Server::~Server()
{
	close(_fd);
}

void Server::setHost(const std::string& host)
{
	_host = host;
}

void Server::addErrorPage(const int& code, const std::string& page)
{
	_errorPages[code] = page;
}

void Server::setPort(const int& port)
{
	_port = port;
}

void Server::setMaxAllowedClientRequestSize(size_t size)
{
	_maxAllowedClientRequestSize = size;
}

void Server::addLocation(const Location& location)
{
	_locations.push_back(location);
}

void Server::setFd(const int& fd)
{
	_fd = fd;
}

const std::string& Server::getHost() const
{
	return (_host);
}

const int& Server::getPort() const
{
	return (_port);
}

int Server::getFd() const
{
	return (_fd);
}

std::vector<Location>& Server::getLocations() {
	return _locations;
}

std::map<int, std::string>& Server::getErrorPages() {
	return _errorPages;
}

ssize_t Server::getMaxRequestSize() const {
	return _maxAllowedClientRequestSize.getSize();
}

static void resuseSocketAddr(const int &fd)
{
	const int yes = 1;
	
	if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) < 0)
		throw std::runtime_error("Can't reuse socket address");
}

static struct addrinfo *getAddressInfo(const int &port, const std::string &host)
{
	struct addrinfo hints = {};
	struct addrinfo *res = NULL;

    hints.ai_family = SOCKET_DOMAIN;
    hints.ai_socktype = SOCKET_TYPE;

	int status = getaddrinfo(host.c_str(), intToString(port).c_str(), &hints, &res);
    if (status)
	{
		freeaddrinfo(res);
        throw std::runtime_error(std::string("Error: ") + gai_strerror(status));
	}
    return (res);
}

void Server::setup()
{
	const int fd = socket(SOCKET_DOMAIN, SOCKET_TYPE, 0);

	if (fd < 0)
		throw std::runtime_error("Socket creation failed");
	setFd(fd);
	resuseSocketAddr(fd);

	struct addrinfo *res = getAddressInfo(_port, _host);
	struct addrinfo *p = res;

	for (p = res; p != NULL; p = p->ai_next) {
		if (bind(fd, p->ai_addr, p->ai_addrlen) >= 0)
			break ;
	}
	if (!p)
	{
		freeaddrinfo(res);
		throw std::runtime_error(std::string("Socket bind failed: ") + strerror(errno));
	}
	freeaddrinfo(res);

	if (listen(fd, SOMAXCONN) < 0)
		throw std::runtime_error("Socket listen failed");
}


bool Server::isLocationExists(const std::string &url)
{
	for (std::vector<Location>::iterator it = _locations.begin(); it != _locations.end(); ++it)
	{
		if (it->getUrl() == url)
			return (true);
	}
	return (false);
}