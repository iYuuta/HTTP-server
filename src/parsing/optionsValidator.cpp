#include "../../includes/parser.hpp"

bool validateOneArg(const std::vector<Token>::iterator& it)
{
	if ((it + 1)->getToken() == Semicolon)
		return (true);
	std::cerr << RED << "Unsupported multiple arguments for key: " << (it - 1)->getKey() << RESET << std::endl;
	return (false);
}

bool validateArgBody(const std::vector<Token>::iterator& it)
{
	if ((it + 1)->getToken() == BracketStart)
		return (true);
	std::cerr << RED << "key: " << (it - 1)->getKey() << " should have a body" << RESET << std::endl;
	return (false);
}

bool isValidPort(const std::string& str, int& port)
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

bool validateMultiArgs(const std::vector<Token>::iterator& it, int args)
{
	int size = 1;

	while ((it + size)->getToken() != Semicolon)
		size++;
	if (size != args)
		std::cerr << RED << "key: " << (it - 1)->getKey() << " requires " << args << " arguments" << RESET << std::endl;
	return (size == args);
}

bool validateAtLeast(const std::vector<Token>::iterator& it, int min)
{
	int size = 1;

	while ((it + size)->getToken() != Semicolon)
		size++;
	if (size < min)
	{
		std::cerr << RED << "key: " << (it - 1)->getKey() << " requires at least " << min << " arguments" << RESET << std::endl;
		return (false);
	}
	return (true);
}
