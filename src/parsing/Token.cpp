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


bool validateTokens(std::vector<Token>& tokens)
{
	if (tokens.empty())
		return (std::cerr << RED << "Empty config file!!" << RESET << std::endl, false);
	std::vector<Token>::iterator it = tokens.begin();
	const std::vector<Token>::iterator end = tokens.end();
	ssize_t						brackets = 0;
	if (it->getToken() != Key)
		return (std::cerr << RED << "Invalid config start: " << it->getKey() << RESET << std::endl, false);
	while (it != end)
	{
		std::vector<Token>::iterator prev = it - 1;
		std::vector<Token>::iterator next = it + 1;

		if (it->getToken() == BracketStart && prev->getToken() != Key && prev->getToken() != Value)
			return (std::cerr << RED << "Invalid brackets start after: " << prev->getKey() << RESET << std::endl, false);
		if (it->getToken() == Semicolon && prev->getToken() != Value)
			return (std::cerr << RED << "Invalid value for key: " << prev->getKey() << RESET << std::endl, false);
		if (it->getToken() == Key && next == end)
			return (std::cerr << RED << "Invalid value for key: " << it->getKey() << RESET << std::endl, false);
		if (it->getToken() == BracketStart)
			brackets++;
		else if (it->getToken() == BracketEnd)
			brackets--;
		++it;
	}
	if (brackets != 0)
		return (std::cerr << RED << "Invalid brackets start/end" << RESET << std::endl, false);
	return (true);
}
