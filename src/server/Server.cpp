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

static void asignHost(sockaddr_in &address, const int &port, const std::string &host)
{
	struct addrinfo hints = {};
	struct addrinfo *res;

    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

	address.sin_family = AF_INET;
	address.sin_port = htons(port);

	int status = getaddrinfo(host.c_str(), NULL, &hints, &res);

    if (status != 0)
	{
   		freeaddrinfo(res);
        throw std::runtime_error(std::string("Error: ") + gai_strerror(status));
	}

	address.sin_addr = ((struct sockaddr_in*)res->ai_addr)->sin_addr;

    freeaddrinfo(res);
}

void Server::setup()
{
	sockaddr_in _address;

	const int fd = socket(AF_INET, SOCK_STREAM, 0);

	if (fd < 0)
		throw std::runtime_error("Socket creation failed");

	setFd(fd);
	
	resuseSocketAddr(fd);

	asignHost(_address, _port, _host);

	if (bind(fd, (sockaddr*)&_address, sizeof(_address)) < 0)
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