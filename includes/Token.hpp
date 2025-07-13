# ifndef TOKEN_HPP
# define TOKEN_HPP

# include "Server.hpp"
# include <vector>
# include <sstream>

class Token
{
    public:
        Token(const std::string &key);
    private:
        std::string _key;
        int         token;
};

std::vector<Token>  splitTokens(const std::string &s);


#endif