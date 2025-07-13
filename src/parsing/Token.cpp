# include "../../includes/Token.hpp"

Token::Token(const std::string &key): _key(key), token(-1)
{
}

std::vector<Token> splitTokens(const std::string &s)
{
	std::vector<Token> tokens;
	std::stringstream ss(s);
	std::string token;
	while (ss >> token) {
		tokens.push_back(Token(token));
	}
	return tokens;
}