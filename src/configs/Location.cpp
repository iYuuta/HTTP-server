#include "../../includes/Location.hpp"

Location::Location() : _autoindex(false)
{
}

void Location::setUrl(const std::string& url)
{
	_url = url;
}

void Location::addMethod(const HttpRequestMethod& method)
{
	_methods.push_back(method);
}

void Location::setRoute(const std::string& route)
{
	_route = route;
}

void Location::setAutoIndex(const bool& autoindex)
{
	_autoindex = autoindex;
}

void Location::setIndex(const std::string& index)
{
	_index = index;
}
