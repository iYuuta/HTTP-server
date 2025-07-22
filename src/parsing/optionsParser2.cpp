#include "../../includes/parser.hpp"
#include "../../includes/utils.hpp"

bool parseLocationMethods(Location& location, std::vector<Token>::iterator& it)
{
	while (it->GetToken() != Semicolon)
	{
		if (it->GetKey() == "GET")
			location.addMethod(GET);
		else if (it->GetKey() == "DELETE")
			location.addMethod(DELETE);
		else if (it->GetKey() == "POST")
			location.addMethod(POST);
		else
			return (std::cerr << "Unknown method: " << it->GetKey() << std::endl, false);
		++it;
	}
	return (true);
}

bool parseLocationRoot(Location& location, std::vector<Token>::iterator& it)
{
	if (!validateOneArg(it))
		return (false);
	location.SetRoute(it->GetKey());
	return (true);
}

bool parseLocationAutoindex(Location& location, std::vector<Token>::iterator& it)
{
	if (!validateOneArg(it))
		return (false);
	if (it->GetKey() == "on")
		location.SetAutoIndex(true);
	else if (it->GetKey() == "off")
		location.SetAutoIndex(false);
	else
		return (std::cerr << "Unknown value " << it->GetKey() << " for key " << (it - 1)->GetKey() << std::endl, false);
	return (true);
}

bool parseLocationIndex(Location& location, std::vector<Token>::iterator& it)
{
	if (!validateOneArg(it))
		return (false);
	location.SetIndex(it->GetKey());
	return (true);
}

bool parseLocationUploadStore(Location& location, std::vector<Token>::iterator& it)
{
	if (!validateOneArg(it))
		return (false);
	location.SetUploadStore(it->GetKey());
	return (true);
}

bool parseLocationCgiExt(Location& location, std::vector<Token>::iterator& it)
{
	if (!validateOneArg(it))
		return (false);
	location.SetCgiExt(it->GetKey());
	return (true);
}

bool parseLocationReturn(Location& location, std::vector<Token>::iterator& it)
{
	if (!validateMultiArgs(it, 2))
		return (false);
	try
	{
		const unsigned long value = atoiul(it->GetKey());
		if (value >= 600 || value < 100)
			return (std::cerr << "Invalid http code " << value << std::endl, false);
		location.SetReturn(value, (++it++)->GetKey());
	}
	catch (std::exception& _)
	{
		return (std::cerr << "Invalid http code " << it->GetKey() << std::endl, false);
	}
	return (true);
}
