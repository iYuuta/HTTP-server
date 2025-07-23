# ifndef SERVER_HPP
# define SERVER_HPP

# include <iostream>
# include <vector>
# include <map>

# include "Location.hpp"
# include "Size.hpp"
# include "Client.hpp"

#define PORT_MAX_VALUE 65535

class Server
{
	private:
		std::string							  		_host;
		int									  		_port;
		std::string							  		_name;
		std::map<int, std::string>					_errorPages;
		Size										_maxAllowedClientRequestSize;
		std::vector<Location>						_locations;


		int									  		_fd;
	public:
		std::vector<int>							_client_fds;
		std::map<int, Client*>						_clients;
		Server( );
		void				setHost(const std::string &host);
		void				setName(const std::string &name);
		void				addErrorPage(const int &code, const std::string &page);
		void				setPort(const int &port);
		const std::string   &getHost() const;
		const int			&getPort() const;
		void				setMaxAllowedClientRequestSize(size_t size);
		void				addLocation(const Location &location);
	
		void tempserver(void);
};

bool parseHostAndPort( Server &server, std::vector<std::string> &content, size_t &i );
bool parseMaxRequestSize( Server &server, std::vector<std::string> &content, size_t &i );

#endif
