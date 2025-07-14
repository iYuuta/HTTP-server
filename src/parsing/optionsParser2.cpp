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
			return (std::cerr << "Unknown method: " << it->getKey() << std::endl, false);
		++it;
	}
	return (true);
}

bool parseLocationRoot(Location& location, std::vector<Token>::iterator& it)
{
	if (!validateOneArg(it))
		return (false);
	location.setRoute(it->getKey());
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
		return (std::cerr << "Unknown value " << it->getKey() << " for key " << (it - 1)->getKey() << std::endl, false);
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

bool parseLocationReturn(Location& location, std::vector<Token>::iterator& it)
{
	if (!validateMultiArgs(it, 2))
		return (false);
	try
	{
		const unsigned long value = atoiul(it->getKey());
		if (value >= 600 || value < 100)
			return (std::cerr << "Invalid http code " << value << std::endl, false);
		location.setReturn(value, (++it++)->getKey());
	}
	catch (std::exception& _)
	{
		return (std::cerr << "Invalid http code " << it->getKey() << std::endl, false);
	}
	return (true);
}
