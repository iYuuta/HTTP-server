#include "../../includes/Location.hpp"

Location::Location() : _autoindex(false)
{
}

void Location::SetUrl(const std::string& url)
{
	_url = url;
}

void Location::addMethod(const enums& method)
{
	_methods.push_back(method);
}

void Location::SetRoute(const std::string& route)
{
	_route = route;
}

void Location::SetAutoIndex(const bool& autoindex)
{
	_autoindex = autoindex;
}

void Location::SetIndex(const std::string& index)
{
	_index = index;
}

void Location::SetUploadStore(const std::string& store)
{
	_uploadStore = store;
}

void Location::SetReturn(const int& code, const std::string& page)
{
	this->_return = std::make_pair(code, page);
}

void Location::SetCgiExt(const std::string& ext)
{
	_cgiExt = ext;
}
