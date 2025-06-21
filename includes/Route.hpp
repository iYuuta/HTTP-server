# ifndef ROUTE_HPP
# define ROUTE_HPP

# include "methods/HttpRequestMethod.hpp"
# include <vector>
# include <iostream>

class Route
{
    private:
        std::vector<HttpRequestMethod> _allowedRequestMethods;
        // TODO http redirect
        std::string             _root;
        std::string             _index;
        bool                    _allowDirectoryListing;
        // TODO CGI ...
};


# endif