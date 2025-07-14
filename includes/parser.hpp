# ifndef PARSER_HPP
# define PARSER_HPP

#include "Server.hpp"
#include "Token.hpp"
#include <errno.h>

bool isValidPort(const std::string &str, int &port);
bool validateOneArg(const std::vector<Token>::iterator& it);
bool validateArgBody(const std::vector<Token>::iterator& it);
bool validateMultiArgs(const std::vector<Token>::iterator& it, int args);
bool parseListen(Server& server, std::vector<Token>::iterator& it);
bool parseClientMaxBodySize(Server& server, std::vector<Token>::iterator& it);
bool parseServerName(Server& server, std::vector<Token>::iterator& it);
bool parseErrorPage(Server& server, std::vector<Token>::iterator& it);
bool parseLocation(Server& server, std::vector<Token>::iterator& it);
bool parseLocationMethods(Location& location, std::vector<Token>::iterator& it);
bool parseLocationRoot(Location& location, std::vector<Token>::iterator& it);
bool parseLocationAutoindex(Location& location, std::vector<Token>::iterator& it);
bool parseLocationIndex(Location& location, std::vector<Token>::iterator& it);

#endif
