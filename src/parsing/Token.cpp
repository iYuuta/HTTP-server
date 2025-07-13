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


void tokenization(std::vector<Token>& tokens)
{
	std::vector<Token>::iterator start = tokens.begin();
	std::vector<Token>::iterator it = start;

	while (it != tokens.end())
	{
		if (it->getKey() == "{")
			it->setToken(BracketStart);
		else if (it->getKey() == "}")
			it->setToken(BracketEnd);
		else if (it->getKey() == ";")
			it->setToken(Semicolon);
		else if (it != start && ((it - 1)->getToken() == Key || (it - 1)->getToken() == Value))
			it->setToken(Value);
		else
			it->setToken(Key);
		++it;
	}
}
