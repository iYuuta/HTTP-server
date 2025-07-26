# ifndef HTTP_SERVER_HPP
# define HTTP_SERVER_HPP

# include "Config.hpp"
#include <poll.h>

class HttpServer
{
	private:
		Config&				_config;
		std::vector<pollfd> _pollFds;

		void clean();
		void listenAll();
		void setupAll();
		void removePollFd(const pollfd &pfd);
		void newPollFd(int fd, short events);
		void handleNewConnection(Server &server, pollfd& pollFd);
		void handleClientRequest(pollfd& pollFd);
		void handleClientResponse(pollfd& pollFd);
	public:
		HttpServer(Config& config);

		bool startAll();
};

#endif
