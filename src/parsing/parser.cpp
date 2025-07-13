#include "../../includes/Config.hpp"
#include "../../includes/Token.hpp"
#include "../../includes/utils.hpp"

static std::string readConfig(std::ifstream &in)
{
	std::string out = "";
	const std::string special = "{};";
	char ch;
	while (in.get(ch)) {
		if (ch != '\n') {
			if (special.find(ch) != std::string::npos)
			{
				out += " ";
				out += ch;
				out += " ";
			}
			else
				out += ch;
		}
	}
	return (out);
}

bool parseServers(std::ifstream& in, Config& conf)
{
	const std::string content = readConfig(in);
	std::vector<Token> tokens = splitTokens(content);
	tokenization(tokens);
	if (!validateTokens(tokens))
		return (false);

	// for (auto& token : tokens)
	// {
	// 	std::cout << token.getKey() << " | " << token.getToken() << std::endl;
	// }
	conf.test();
	return (true);
}
