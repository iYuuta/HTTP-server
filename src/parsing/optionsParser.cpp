#include "../../includes/parser.hpp"


bool parseListen(Server& server, std::vector<Token>::iterator& it)
{
	while (it->getToken() == Value)
		++it;
	return (true);
}
