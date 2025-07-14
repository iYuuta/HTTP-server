# include "../../includes/utils.hpp"
    
std::vector<std::string> split(const std::string& s, const char delimiter)
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

std::vector<std::string> splitNumber(const std::string& s)
{
    std::vector<std::string> tokens;
    if (!std::isdigit(s[0]))
        return (tokens);
    for (size_t i = 0; i < s.size(); ++i) {
        if (!std::isdigit(s[i])) {
            tokens.push_back(s.substr(0, i));
            tokens.push_back(s.substr(i));
            break;
        }
    }
    return tokens;
}
