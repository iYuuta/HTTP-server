#include "../../includes/parser.hpp"

static bool requiredValue(const std::string &key)
{
	std::cerr <<  key << " is required!" << std::endl;
	return (false);
}

bool validateServer(Server &server)
{
	if (server.getHost().empty())
		return (requiredValue("interface"));
	if (server.getPort() == -1)
		return (requiredValue("port"));
	if (server.getMaxRequestSize() == -1)
		return (requiredValue("client_max_body_size"));
	return (true);
}

static bool isValidRedirect(int status)
{
	return (status >= 301 && status <= 308);
}

bool validateLocation(Location &location)
{
	if (location.isRedirect() && !isValidRedirect(location.getReturn().first))
		return (std::cerr << "Invalid redirect code (301~308)" << std::endl, false);
	return (true);
}