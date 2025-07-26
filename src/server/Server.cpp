#include "../../includes/Server.hpp"

Server::Server(): _fd(-1), _port(-1)
{
}

void Server::setHost(const std::string& host)
{
	_host = host;
}

void Server::addErrorPage(const int& code, const std::string& page)
{
	_errorPages[code] = page;
}

void Server::setName(const std::string& name)
{
	_name = name;
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

size_t Server::getMaxRequestSize() const {
	return _maxAllowedClientRequestSize.getSize();
}

void Server::setup()
{
	_address.sin_family = AF_INET;
	_address.sin_port = htons(_port);
	_address.sin_addr.s_addr = INADDR_ANY;

	const int fd = socket(AF_INET, SOCK_STREAM, 0);
	if (fd < 0)
		throw std::runtime_error("Socket creation failed");

	int yes = 1;
	if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) < 0) {
		perror("setsockopt");
		close(fd);
		exit(EXIT_FAILURE);
	}
	setFd(fd);
	if (bind(fd, (sockaddr*)&_address, sizeof(_address)) < 0)
		throw std::runtime_error("Socket bind failed");

	if (listen(fd, SOMAXCONN) < 0)
		throw std::runtime_error("Socket listen failed");
}

const sockaddr_in& Server::getSocketAddress() const
{
	return (_address);
}
