#include "../../includes/parser.hpp"
#include "../../includes/utils.hpp"

static bool validateOneArg(std::vector<Token>::iterator& it)
{
	if ((it + 1)->getToken() == Semicolon)
		return (true);
	std::cerr << "Unsupported multiple arguments for key: " << (it - 1)->getKey() << std::endl;
	return (false);
}

static bool  isValidPort(const std::string &str, int &port)
{
	if (str.empty() || str.size() > 5)
		return (false);
	if (str[0] == '0')
		return (!str[1]);
	for (size_t i = 0; i < str.size(); i++)
	{
		if (!std::isdigit(str[i]))
			return (false);
	}
	port = std::atoi(str.c_str());
	return (port <= PORT_MAX_VALUE);
}

bool parseListen(Server& server, std::vector<Token>::iterator& it)
{
	int port;
	if (!validateOneArg(it))
		return (false);
	const std::vector<std::string> s = split(it->getKey(), ':');
	if (s.size() != 2)
		return (std::cerr << "interface:port required for the listen value" << std::endl, false);
	if (!isValidPort(s.at(1), port))
		return (std::cerr << "Invalid port: " << s.at(1) << std::endl, false);
	server.setHost(s.at(0));
	server.setPort(port);
	return (true);
}

bool parseClientMaxBodySize(Server& server, std::vector<Token>::iterator& it)
{
	if (!validateOneArg(it))
		return (false);
	const std::vector<std::string> s = splitNumber(it->getKey());
	if (s.size() != 2)
		return (std::cerr << "Invalid " << (it - 1)->getKey() << std::endl, false);
	if (s.at(1) != "KB")
		return (std::cerr << (it - 1)->getKey() << " must be in KB" << std::endl, false);
	errno = 0;
	char *ptr;
	const unsigned long value = std::strtoul(s.at(0).c_str(), &ptr, 10);
	if (errno != 0 || *ptr != '\0')
		return (errno = 0, std::cerr << "Invalid value " << s.at(0) << std::endl, false);
	server.setMaxAllowedClientRequestSize(Size(value));
	return (true);
}
