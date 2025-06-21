# include "../../includes/utils.hpp"

bool operator>>(std::ifstream &in, std::string &out)
{
    std::string special = "{};";
    char ch;
    while (in.get(ch)) {
        if (ch != '\n') {
            if (special.find(ch) != std::string::npos)
            {
                out += " ";
                out += ch;
                out += " ";
            }
            else
                out += ch;
        }
    }
    return (true);
}
    
