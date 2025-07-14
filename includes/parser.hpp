# ifndef PARSER_HPP
# define PARSER_HPP

#include "Server.hpp"
#include "Token.hpp"

bool parseListen(Server& server, std::vector<Token>::iterator& it);

#endif
