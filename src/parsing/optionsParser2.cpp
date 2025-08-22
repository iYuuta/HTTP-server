#include "../../includes/parser.hpp"
#include "../../includes/utils.hpp"

bool parseLocationMethods(Location& location, std::vector<Token>::iterator& it)
{
	while (it->getToken() != Semicolon)
	{
		if (it->getKey() == "GET")
			location.addMethod(Get);
		else if (it->getKey() == "DELETE")
			location.addMethod(Delete);
		else if (it->getKey() == "POST")
			location.addMethod(Post);
		else
			return (std::cerr << RED << "Unknown method: " << it->getKey() << RESET << std::endl, false);
		++it;
	}
	return (true);
}

bool parseLocationRoot(Location& location, std::vector<Token>::iterator& it)
{
	if (!validateOneArg(it))
		return (false);
	location.setRoute(removeLast(it->getKey(), '/'));
	return (true);
}

bool parseLocationAutoindex(Location& location, std::vector<Token>::iterator& it)
{
	if (!validateOneArg(it))
		return (false);
	if (it->getKey() == "on")
		location.setAutoIndex(true);
	else if (it->getKey() == "off")
		location.setAutoIndex(false);
	else
		return (std::cerr << RED << "Unknown value " << it->getKey() << " for key " << (it - 1)->getKey() << RESET << std::endl, false);
	return (true);
}

bool parseLocationIndex(Location& location, std::vector<Token>::iterator& it)
{
	if (!validateOneArg(it))
		return (false);
	location.setIndex(it->getKey());
	return (true);
}

bool parseLocationUploadStore(Location& location, std::vector<Token>::iterator& it)
{
	if (!validateOneArg(it))
		return (false);
	location.setUploadStore(it->getKey());
	return (true);
}

bool parseLocationCgiExt(Location& location, std::vector<Token>::iterator& it)
{
	while (it->getToken() != Semicolon)
	{
		location.addCgiExt(it->getKey());
		++it;
	}
	return (true);
}

bool parseLocationReturn(Location& location, std::vector<Token>::iterator& it)
{
	if (!validateMultiArgs(it, 2))
		return (false);
	try
	{
		const long long value = atoill(it->getKey());
		if (value >= 600 || value < 100)
			return (std::cerr << RED << "Invalid http code " << value << RESET << std::endl, false);
		location.setReturn(static_cast<int>(value), (++it++)->getKey());
	}
	catch (std::exception& _)
	{
		return (std::cerr << RED << "Invalid http code" << RESET << std::endl, false);
	}
	return (true);
}
