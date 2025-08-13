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
	return (true);
}

bool validateLocation(Location &location)
{
	(void) location;
	return (true);
}