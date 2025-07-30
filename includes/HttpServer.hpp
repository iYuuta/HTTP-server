# ifndef HTTP_SERVER_HPP
# define HTTP_SERVER_HPP

# include "Config.hpp"
#include <poll.h>

class HttpServer
{
	private:
		Config&					_config;
		std::vector<pollfd>		_pollFds;
		std::map<int, Client*>	_clients;

		void clean();
		void listen();
		void setupAll();
		void removePollFd(const pollfd &pfd);
		void newPollFd(const int &fd, const short &events);
		void handleNewConnection(pollfd& pollFd);
		void handleClientRequest(pollfd& pollFd);
		void handleClientResponse(pollfd& pollFd);
		Server& getServerByFd(const int &fd);
		void insertNewClient(const int& clientId, Server &server);
		bool isClientExists(const int& clientId);
		Client &getClient(const int& clientId);
	public:
		HttpServer(Config& config);
		void closeFd(std::vector<pollfd>::iterator it);
		bool startAll();
};

#endif
