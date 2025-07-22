# ifndef TOKEN_HPP
# define TOKEN_HPP

# include "Server.hpp"
# include <vector>
# include <sstream>
# include <cstdlib>


enum Tokens
{
	BracketStart,
	BracketEnd,
	Semicolon,
	Key,
	Value
};

class Token
{
	public:
		Token(const std::string& key);
		const std::string& GetKey() const;
		const int& GetToken() const;
		void SetToken(const int& token);

	private:
		std::string _key;
		int _token;
};

std::vector<Token> splitTokens(const std::string& s);
void tokenization(std::vector<Token>& tokens);
bool validateTokens(std::vector<Token>& tokens);


#endif
