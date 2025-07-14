#include "../../includes/parser.hpp"
#include "../../includes/utils.hpp"

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

bool parseServerName(Server& server, std::vector<Token>::iterator& it)
{
	if (!validateOneArg(it))
		return (false);
	server.setName(it->getKey());
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
	try
	{
		const unsigned long value = atoiul(s.at(0));
		server.setMaxAllowedClientRequestSize(Size(value));
	}
	catch (std::exception& e)
	{
		return (std::cerr << "Invalid value " << s.at(0) << std::endl, false);
	}
	return (true);
}

bool parseErrorPage(Server& server, std::vector<Token>::iterator& it)
{
	if (!validateMultiArgs(it, 2))
		return (false);
	try
	{
		const unsigned long value = atoiul(it->getKey());
		if (value >= 600 || value < 400)
			return (std::cerr << "Invalid http error code " << value << std::endl, false);
		server.addErrorPage(value, (++it++)->getKey());
	}
	catch (std::exception& e)
	{
		return (std::cerr << "Invalid http code " << it->getKey() << std::endl, false);
	}
	// server.setName(it->getKey());
	return (true);
}
