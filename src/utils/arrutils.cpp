# include "../../includes/utils.hpp"

int indexOf(std::string arr[], size_t arrSize, std::string toFind)
{
    int i = 0;
    while (i < arrSize)
    {
        if (arr[i] == toFind)
            return (i);
        i++;
    }
    return (-1);
}
