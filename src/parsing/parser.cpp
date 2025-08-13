#include "../../includes/parser.hpp"

#include "../../includes/Config.hpp"
#include "../../includes/Token.hpp"
#include "../../includes/utils.hpp"
# include <iostream>

static std::string readConfig(std::ifstream& in)
{
	std::string out;
	const std::string special = "{};";
	char ch;
	while (in.get(ch))
	{
		if (ch != '\n')
		{
			if (special.find(ch) != std::string::npos)
			{
				out += " ";
				out += ch;
				out += " ";
			}
			else
				out += ch;
		}
	}
	return (out);
}

static bool parseOption(std::vector<Token>::iterator& it, Server& server)
{
	const std::string keys[5] = {"listen", "client_max_body_size", "server_name", "error_page", "location"};
	bool (*fn[5])(Server& server, std::vector<Token>::iterator& it) = {
		::parseListen, ::parseClientMaxBodySize, ::parseServerName, ::parseErrorPage, ::parseLocation,
	};
	for (size_t i = 0; i < 5; i++)
	{
		if (keys[i] == it->getKey())
			return (fn[i](server, ++it));
	}
	std::cerr << "Unknown key: " << it->getKey() << std::endl;
	return (false);
}

static bool parseServer(std::vector<Token>::iterator& it, Config& conf)
{
	Server server;
	size_t brackets = 1;

	if (it->getKey() != "server")
		return (std::cerr << "Invalid key in this context: " << it->getKey() << std::endl, false);
	it += 2;
	while (brackets)
	{
		if (it->getToken() == Key)
		{
			if (!parseOption(it, server))
				return (false);
			continue ;
		}
		if (it->getToken() == BracketStart)
			brackets++;
		else if (it->getToken() == BracketEnd)
			brackets--;
		++it;
	}
	if (!validateServer(server))
		return (false);
	conf.addServer(server);
	return (true);
}

bool parseServers(std::ifstream& in, Config& conf)
{
	const std::string content = readConfig(in);
	std::vector<Token> tokens = splitTokens(content);
	tokenization(tokens);
	if (!validateTokens(tokens))
		return (false);
	std::vector<Token>::iterator it = tokens.begin();
	while (it != tokens.end())
	{
		if (!parseServer(it, conf))
			return (false);
	}
	return (true);
}
