# include "../../includes/Config.hpp"
# include "../../includes/utils.hpp"

static int getTagEnd(std::vector<std::string> content, size_t start)
{
    int     end = start + 1;
    int     openings = 0;
    size_t  size = content.size();

    if (content[start + 1] != "{")
        return (std::cerr << "Tag '" << content[start] << "' has no start" << std::endl, -1);
    while (end < size)
    {
        if (content[end] == "{")
            openings++;
        else if (content[end] == "}")
            openings--;
        if (openings == 0)
            return (end);
        end++;
    }
    std::cerr << "Tag '" << content[start] << "' has no ending" << std::endl;
    return (-1);
}


static bool parseOptionOneArg(std::vector<std::string> content, size_t &start, int optionIndex)
{
    if (content[start] == ";")
        return (std::cerr << "You forgot an argument for option '" << content[start - 1] << "'" << std::endl, false);
    if (content[start + 1] != ";")
        return (std::cerr << "Only one argument is allowed for option '" << content[start - 1] << "'" << std::endl, false);
    
    return (true);
}

static bool parseOption(std::vector<std::string> content, size_t &start, int optionIndex)
{
    switch (optionIndex)
    {
        case 0:
        case 3:
            parseOptionOneArg(content, start, optionIndex);
            break;
        case 1:
        case 2:
            std::cout << "MULTI ARG\n";
            break;
        case 4:
            std::cout << "BODY\n";
            break; 
        default:
            return (false);
    }
    return (true);
}

static bool parseServer(std::vector<std::string> content, size_t &i, Config &config)
{
    int end = getTagEnd(content, i);
    if (end == -1)
        return (false);
    std::string keys[5] = {"listen", "server_name", "error_page", "client_max_body_size", "location"};
    Server server;
    i += 2;
    while (i < end - 1)
    {
        int optionIndex = indexOf(keys, 5, content[i]);
        if (optionIndex == -1)
            return (std::cerr << "Invalid key '" << content[i] << "'" << std::endl, false);
        if (!parseOption(content, ++i, optionIndex))
            return (false);
        i++;
    }
    config.addServer(server);
    return (i++, true);
}

bool parseServers(std::ifstream& in, Config &conf)
{
    std::string content;
    in >> content;
    std::vector<std::string> a = split(content);

    for (size_t i = 0; i < a.size(); i++) {
        if (a[i] == "server")
        {
            if (!parseServer(a, i, conf))
                return (false);
        }
        else
	        return (std::cerr << "Uknown key: " << a[i] << std::endl, false);
    }
    conf.test();
	return (true);
}
