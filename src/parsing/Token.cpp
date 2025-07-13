# include "../../includes/Token.hpp"

Token::Token(const std::string& key): _key(key), _token(-1)
{
}

std::vector<Token> splitTokens(const std::string& s)
{
	std::vector<Token> tokens;
	std::stringstream ss(s);
	std::string token;

	while (ss >> token)
	{
		tokens.push_back(Token(token));
	}
	return tokens;
}

const std::string& Token::getKey() const
{
	return (_key);
}

const int& Token::getToken() const
{
	return (_token);
}

void Token::setToken(const int& token)
{
	_token = token;
}


/**
 *
 * @return Returns true on success; if it returns false, an error message will be printed to stderr to indicate the issue.
 */
bool tokenization(std::vector<Token>& tokens)
{
	for (std::vector<Token>::iterator i = tokens.begin(); i != tokens.end(); ++i)
	{

	}
	return (true);
}
