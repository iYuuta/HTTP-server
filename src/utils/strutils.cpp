# include "../../includes/utils.hpp"

std::vector<std::string> splitWhitespaces(const std::string& s)
{
    std::vector<std::string> tokens;
    std::stringstream ss(s);
    std::string token;
    while (ss >> token) {
        tokens.push_back(token);
    }
    return tokens;
}
    
std::vector<std::string> split(const std::string& s, char delimiter)
{
    std::vector<std::string> tokens;
    size_t  start = 0;
    size_t  end;

    while ((end = s.find(delimiter, start)) != std::string::npos) {
        tokens.push_back(s.substr(start, end - start));
        start = end + 1;
    }

    tokens.push_back(s.substr(start));
    return tokens;
}
