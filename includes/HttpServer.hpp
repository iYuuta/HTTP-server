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
		void setup(Server& server);

	public:
		HttpServer(Config& config);

		bool startAll();
};

#endif
