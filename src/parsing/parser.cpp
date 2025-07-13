#include "../../includes/Config.hpp"
#include "../../includes/Token.hpp"
#include "../../includes/utils.hpp"

bool parseServers(std::ifstream& in, Config &conf)
{
    std::string content;
    in >> content;
    std::vector<Token> tokens = splitTokens(content);

    // TODO: HH
    conf.test();
	return (true);
}
