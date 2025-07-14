#include "../../includes/parser.hpp"

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

