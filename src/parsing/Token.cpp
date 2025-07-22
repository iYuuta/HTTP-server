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

const std::string& Token::GetKey() const
{
	return (_key);
}

const int& Token::GetToken() const
{
	return (_token);
}

void Token::SetToken(const int& token)
{
	_token = token;
}


void tokenization(std::vector<Token>& tokens)
{
	std::vector<Token>::iterator start = tokens.begin();
	std::vector<Token>::iterator it = start;

	while (it != tokens.end())
	{
		if (it->GetKey() == "{")
			it->SetToken(BracketStart);
		else if (it->GetKey() == "}")
			it->SetToken(BracketEnd);
		else if (it->GetKey() == ";")
			it->SetToken(Semicolon);
		else if (it != start && ((it - 1)->GetToken() == Key || (it - 1)->GetToken() == Value))
			it->SetToken(Value);
		else
			it->SetToken(Key);
		++it;
	}
}


bool validateTokens(std::vector<Token>& tokens)
{
	if (tokens.empty())
		return (std::cerr << "Empty configs!!" << std::endl, false);
	std::vector<Token>::iterator it = tokens.begin();
	const std::vector<Token>::iterator end = tokens.end();
	ssize_t						brackets = 0;
	if (it->GetToken() != Key)
		return (std::cerr << "Invalid config start: " << it->GetKey() << std::endl, false);
	while (it != end)
	{
		std::vector<Token>::iterator prev = it - 1;
		std::vector<Token>::iterator next = it + 1;

		if (it->GetToken() == BracketStart && prev->GetToken() != Key && prev->GetToken() != Value)
			return (std::cerr << "Invalid brackets start after: " << prev->GetKey() << std::endl, false);
		if (it->GetToken() == Semicolon && prev->GetToken() != Value)
			return (std::cerr << "Invalid value for key: " << prev->GetKey() << std::endl, false);
		if (it->GetToken() == Key && next == end)
			return (std::cerr << "Invalid value for key: " << it->GetKey() << std::endl, false);
		if (it->GetToken() == BracketStart)
			brackets++;
		else if (it->GetToken() == BracketEnd)
			brackets--;
		++it;
	}
	if (brackets != 0)
		return (std::cerr << "Invalid brackets start/end" << std::endl, false);
	return (true);
}
