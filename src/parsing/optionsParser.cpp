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
	catch (std::exception& _)
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
	catch (std::exception& _)
	{
		return (std::cerr << "Invalid http code " << it->getKey() << std::endl, false);
	}
	return (true);
}

static bool parseLocationOption(Location& location, std::vector<Token>::iterator& it)
{
	const std::string keys[5] = {"methods", "root", "autoindex", "index"};
	bool (*fn[5])(Location&, std::vector<Token>::iterator&) = {
		::parseLocationMethods, ::parseLocationMethods, ::parseLocationMethods, ::parseLocationMethods
	};
	for (size_t i = 0; i < 5; i++)
	{
		if (keys[i] == it->getKey())
			return (fn[i](location, ++it));
	}
	std::cerr << "Unknown key: " << it->getKey() << std::endl;
	return (false);
}

bool parseLocation(Server& server, std::vector<Token>::iterator& it)
{
	Location location;
	size_t brackets = 1;

	if (!validateArgBody(it))
		return (false);
	location.setUrl(it++->getKey());
	server.addLocation(location);
	++it;
	while (brackets)
	{
		if (it->getToken() == Key)
		{
			if (!parseLocationOption(location, it))
				return (false);
			continue ;
		}
		if (it->getToken() == BracketStart)
			brackets++;
		else if (it->getToken() == BracketEnd)
			brackets--;
		++it;
	}
	server.addLocation(location);
	return (true);
}
