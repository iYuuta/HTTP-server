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

static bool parseServer(std::vector<std::string> content, size_t &i)
{
    int end = getTagEnd(content, i);
    if (end == -1)
        return (false);
    i += 2;
    while (i < end - 1)
    {
        std::cout << content[i] << std::endl;
        i++;
    }
    return (i++, true);
}

bool parseServers(std::ifstream& in, Config conf)
{
    std::string content;
    in >> content;
    std::vector<std::string> a = split(content);

    for (size_t i = 0; i < a.size(); i++) {
        if (a[i] == "server")
        {
            if (!parseServer(a, i))
                return (false);
        }
        else
	        return (std::cerr << "Uknown key: " << a[i] << std::endl, false);
    }
	return (true);
}
