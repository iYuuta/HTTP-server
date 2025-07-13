# ifndef TOKEN_HPP
# define TOKEN_HPP

# include "Server.hpp"
# include <vector>
# include <sstream>

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
		const std::string& getKey() const;
		const int& getToken() const;
		void setToken(const int& token);

	private:
		std::string _key;
		int _token;
};

std::vector<Token> splitTokens(const std::string& s);
void tokenization(std::vector<Token>& tokens);


#endif
