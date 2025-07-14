# ifndef PARSER_HPP
# define PARSER_HPP

#include "Server.hpp"
#include "Token.hpp"
#include <errno.h>

bool parseListen(Server& server, std::vector<Token>::iterator& it);
bool parseClientMaxBodySize(Server& server, std::vector<Token>::iterator& it);
#endif
