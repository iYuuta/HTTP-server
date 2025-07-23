# ifndef HTTP_SERVER_HPP
# define HTTP_SERVER_HPP

# include "Config.hpp"

class HttpServer
{
	private:
		Config& _config;
	public:
		HttpServer(Config& config);

		void startAll() const;
};

#endif
