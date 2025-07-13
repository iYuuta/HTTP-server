# include "../../includes/Config.hpp"
# include "../../includes/utils.hpp"

bool parseServers(std::ifstream& in, Config &conf)
{
    std::string content;
    in >> content;
    std::vector<std::string> a = splitWhitespaces(content);

    // TODO: HH
    conf.test();
	return (true);
}
