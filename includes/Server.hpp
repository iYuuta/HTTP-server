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
		int									  		_fd;
		std::string							  		_name;
		std::vector<std::pair<int, std::string> >	_errorPages;
		size_t										_maxAllowedClientRequestSize;
		std::vector<Location>						_locations;
	public:
		std::vector<int>							_client_fds;
		std::map<int, Client*>						_clients;
		Server( );
		void				SetHost(const std::string &host);
		void				SetName(const std::string &name);
		void				AddErrorPage(const int &code, const std::string &page);
		void				SetPort(const int &port);
		const std::string   &GetHost() const;
		const int			&GetPort() const;
		void				SetMaxAllowedClientRequestSize(size_t size);
		void				AddLocation(const Location &location);
	
		void tempserver(void);
};

bool parseHostAndPort( Server &server, std::vector<std::string> &content, size_t &i );
bool parseMaxRequestSize( Server &server, std::vector<std::string> &content, size_t &i );

#endif
