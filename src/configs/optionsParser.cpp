# include "../../includes/Server.hpp"
# include "../../includes/utils.hpp"

static bool  isValidPort(const std::string &str, int &port)
{
    if (str.empty() || str.size() > 5)
        return (false);
    if (str[0] == '0')
        return (!str[1]);
    for (size_t i = 0; i < str.size(); i++)
    {
        if (!std::isdigit(str[i]))
            return (false);
    }
    port = std::atoi(str.c_str());
    return (port <= PORT_MAX_VALUE);
}

bool parseHostAndPort( Server &server, std::vector<std::string> &content, size_t &i )
{
    std::vector<std::string> list = split(content[i], ':');
    if (list.size() != 2)  
        return (std::cerr << "A valid listen value should look something like 127.0.0.1:8080" << std::endl, false);
    std::string &portStr = list[1];
    int         port = 0;
    if (!isValidPort(portStr, port))
        return (std::cerr << portStr << ": is not a valid port!" << std::endl, false);
    server.setPort(port);
    i++;
    return (true);
}

bool parseMaxRequestSize( Server &server, std::vector<std::string> &content, size_t &i )
{
    std::vector<std::string> list = splitNumber(content[i]);
    if (list.size() != 2)  
        return (std::cerr << "client_max_body_size no valid" << std::endl, false);
    std::cout << list[0] << "\n";
    std::cout << list[1] << "\n";
    return (false);
}
