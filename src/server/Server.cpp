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

static void asignAddressInfo(sockaddr_in &address, const int &port, const std::string &host)
{
	struct addrinfo hints = {};
	struct addrinfo *res;

    hints.ai_family = SOCKET_DOMAIN;
    hints.ai_socktype = SOCKET_TYPE;

	int status = getaddrinfo(host.c_str(), intToString(port).c_str(), &hints, &res);

    if (status)
        throw std::runtime_error(std::string("Error: ") + gai_strerror(status));

	const sockaddr_in* resolvedAddr = reinterpret_cast<sockaddr_in *>(res->ai_addr);

	address.sin_addr = resolvedAddr->sin_addr;
	address.sin_port = resolvedAddr->sin_port;
	address.sin_family = resolvedAddr->sin_family;

    freeaddrinfo(res);
}

void Server::setup()
{
	sockaddr_in _address;

	const int fd = socket(SOCKET_DOMAIN, SOCKET_TYPE, 0);

	if (fd < 0)
		throw std::runtime_error("Socket creation failed");

	setFd(fd);
	
	resuseSocketAddr(fd);

	asignAddressInfo(_address, _port, _host);

	if (bind(fd, reinterpret_cast<sockaddr *>(&_address), sizeof(_address)) < 0)
		throw std::runtime_error(std::string("Socket bind failed: ") + strerror(errno));

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