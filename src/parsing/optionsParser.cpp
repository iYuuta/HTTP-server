#include "../../includes/parser.hpp"
#include "../../includes/utils.hpp"

bool parseListen(Server& server, std::vector<Token>::iterator& it)
{
	int port;
	if (!validateOneArg(it))
		return (false);
	const std::vector<std::string> s = split(it->GetKey(), ':');
	if (s.size() != 2)
		return (std::cerr << "interface:port required for the listen value" << std::endl, false);
	if (!isValidPort(s.at(1), port))
		return (std::cerr << "Invalid port: " << s.at(1) << std::endl, false);
	server.SetHost(s.at(0));
	server.SetPort(port);
	return (true);
}

bool parseServerName(Server& server, std::vector<Token>::iterator& it)
{
	if (!validateOneArg(it))
		return (false);
	server.SetName(it->GetKey());
	return (true);
}

bool parseClientMaxBodySize(Server& server, std::vector<Token>::iterator& it)
{
	if (!validateOneArg(it))
		return (false);
	const std::vector<std::string> s = splitNumber(it->GetKey());
	if (s.size() != 2)
		return (std::cerr << "Invalid " << (it - 1)->GetKey() << std::endl, false);
	if (s.at(1) != "KB")
		return (std::cerr << (it - 1)->GetKey() << " must be in KB" << std::endl, false);
	try
	{
		const unsigned long value = atoiul(s.at(0));
		server.SetMaxAllowedClientRequestSize(value);
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
		const unsigned long value = atoiul(it->GetKey());
		if (value >= 600 || value < 400)
			return (std::cerr << "Invalid http error code " << value << std::endl, false);
		server.AddErrorPage(value, (++it++)->GetKey());
	}
	catch (std::exception& _)
	{
		return (std::cerr << "Invalid http code " << it->GetKey() << std::endl, false);
	}
	return (true);
}

static bool parseLocationOption(Location& location, std::vector<Token>::iterator& it)
{
	const std::string keys[10] = {"methods", "root", "autoindex", "index", "upload_store", "return", "cgi_ext"};
	bool (*fn[10])(Location&, std::vector<Token>::iterator&) = {
		::parseLocationMethods, ::parseLocationRoot, ::parseLocationAutoindex, ::parseLocationIndex,
		::parseLocationUploadStore, ::parseLocationReturn, ::parseLocationCgiExt
	};
	for (size_t i = 0; i < 10; i++)
	{
		if (keys[i] == it->GetKey())
			return (fn[i](location, ++it));
	}
	std::cerr << "Unknown key: " << it->GetKey() << std::endl;
	return (false);
}

bool parseLocation(Server& server, std::vector<Token>::iterator& it)
{
	Location location;
	size_t brackets = 1;

	if (!validateArgBody(it))
		return (false);
	location.SetUrl(it++->GetKey());
	server.AddLocation(location);
	++it;
	while (brackets)
	{
		if (it->GetToken() == Key)
		{
			if (!parseLocationOption(location, it))
				return (false);
			continue ;
		}
		if (it->GetToken() == BracketStart)
			brackets++;
		else if (it->GetToken() == BracketEnd)
			brackets--;
		++it;
	}
	server.AddLocation(location);
	return (true);
}
