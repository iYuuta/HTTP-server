# include "../../includes/utils.hpp"

std::vector<std::string> split(const std::string& s)
{
    std::vector<std::string> tokens;
    std::stringstream ss(s);
    std::string token;
    while (ss >> token) {
        tokens.push_back(token);
    }
    return tokens;
}
    
