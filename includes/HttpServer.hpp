# ifndef HTTP_SERVER_HPP
# define HTTP_SERVER_HPP

# include "Config.hpp"

class HttpServer
{
	private:
		Config& _config;

		void clean();
		void listen();
	public:
		HttpServer(Config& config);

		bool startAll();
};

#endif
