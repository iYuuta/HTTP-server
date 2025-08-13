# ifndef SERVER_HPP
# define SERVER_HPP

# include <iostream>
# include <vector>
# include <map>
#include <netinet/in.h>

# include "Location.hpp"
# include "Size.hpp"
# include "Client.hpp"

#define PORT_MAX_VALUE 65535


class Server
{
	private:
		sockaddr_in _address;
		std::string _host;
		int _fd;
		int _port;
		std::string _name;
		std::map<int, std::string> _errorPages;
		Size _maxAllowedClientRequestSize;
		std::vector<Location> _locations;

	public:
		Server();
		void setHost(const std::string& host);
		void setFd(const int& fd);
		void setName(const std::string& name);
		void addErrorPage(const int& code, const std::string& page);
		void setPort(const int& port);
		const std::string& getHost() const;
		const int& getPort() const;
		void setMaxAllowedClientRequestSize(size_t size);
		void addLocation(const Location& location);
		void insertNewClient(const int& clientId);
		bool isClientExists(const int& clientId);
		void setup();
		int getFd() const;
		std::vector<Location>& getLocations();
		std::map<int, std::string>& getErrorPages();
		size_t getMaxRequestSize() const;
		const sockaddr_in& getSocketAddress() const;
};

bool parseHostAndPort(Server& server, std::vector<std::string>& content, size_t& i);
bool parseMaxRequestSize(Server& server, std::vector<std::string>& content, size_t& i);

#endif
