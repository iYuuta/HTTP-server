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

bool parseClientMaxBodySize(Server& server, std::vector<Token>::iterator& it)
{
	if (!validateOneArg(it))
		return false;

	const std::vector<std::string> s = splitNumber(it->getKey());
	if (s.size() != 2) {
		std::cerr << "Invalid " << (it - 1)->getKey() << std::endl;
		return false;
	}
	const char* str = s.at(0).c_str();
	char* endptr;
	errno = 0;

	unsigned long long baseValue = std::strtoull(str, &endptr, 10);

	if (errno == ERANGE || endptr == str) {
		std::cerr << "Invalid numeric value: " << s.at(0) << std::endl;
		return false;
	}
	if (*endptr != '\0') {
		std::cerr << "Unexpected characters in number: " << s.at(0) << std::endl;
		return false;
	}
	unsigned long long multiplier = 1;
	if (s.at(1) == "B")
		multiplier = 1;
	else if (s.at(1) == "KB")
		multiplier = 1000;
	else if (s.at(1) == "MB")
		multiplier = 1000000;
	else if (s.at(1) == "GB")
		multiplier = 1000000000;
	else {
		std::cerr << (it - 1)->getKey() << " must be in a valid unit (KB, MB, GB)" << std::endl;
		return false;
	}

	if (baseValue > ULLONG_MAX / multiplier) {
		std::cerr << "Client body size is too large" << std::endl;
		return false;
	}
	size_t value = baseValue * multiplier;
	server.setMaxAllowedClientRequestSize(value);
	return true;
}

bool parseErrorPage(Server& server, std::vector<Token>::iterator& it)
{
	if (!validateMultiArgs(it, 2))
		return (false);
	try
	{
		const long long value = atoill(it->getKey());
		if (value >= 600 || value < 400)
			return (std::cerr << "Invalid http error code " << value << std::endl, false);
		server.addErrorPage(static_cast<int>(value), (++it++)->getKey());
	}
	catch (std::exception& _)
	{
		return (std::cerr << "Invalid http code" << std::endl, false);
	}
	return (true);
}

static bool parseLocationOption(Location& location, std::vector<Token>::iterator& it)
{
	const std::string keys[7] = {"methods", "root", "autoindex", "index", "upload_store", "return", "cgi_ext"};
	bool (*fn[7])(Location&, std::vector<Token>::iterator&) = {
		::parseLocationMethods, ::parseLocationRoot, ::parseLocationAutoindex, ::parseLocationIndex,
		::parseLocationUploadStore, ::parseLocationReturn, ::parseLocationCgiExt
	};
	for (size_t i = 0; i < 7; i++)
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
	location.setUrl(removeRepeating(it++->getKey(), '/'));
	if (server.isLocationExists(location.getUrl()))
	{
		std::cerr << "Duplicated location: " << location.getUrl() << std::endl;
		return (false);
	}
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
	if (!validateLocation(location))
		return (false);
	server.addLocation(location);
	return (true);
}
