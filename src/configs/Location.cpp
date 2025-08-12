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

const std::pair<int, std::string>&	Location::getReturn() {
	return _return;
}

bool Location::isRedirect() {
	if (_return.second.empty())
		return false;
	return true;
}


void Location::setIndex(const std::string& index)
{
	_index = index;
}

void Location::setUploadStore(const std::string& store)
{
	_uploadStore = store;
}

void Location::setReturn(const int& code, const std::string& page)
{
	this->_return = std::make_pair(code, page);
}

bool Location::isMethodValid(const HttpRequestMethod& method)
{
	for (std::vector<HttpRequestMethod>::iterator it = _methods.begin(); it != _methods.end(); ++it)
	{
		if (*it == method)
			return true;
	}
	return false;
}

void Location::printMethods()
{
	for (std::vector<HttpRequestMethod>::iterator it = _methods.begin(); it != _methods.end(); ++it)
	{
		switch (*it)
		{
			case Get: std::cout << "Method: GET\n";
				break;
			case Post: std::cout << "Method: POST\n";
				break;
			case Delete: std::cout << "Method: DELETE\n";
				break;
			default: std::cout << "Method: Unsupported\n";
				break;
		}
	}
}

const std::string& Location::getUrl()
{
	return _url;
}

const std::string& Location::getRoute()
{
	return _route;
}

const std::string& Location::getIndex()
{
	return _index;
}

const std::string& Location::getUploadStore()
{
	return _uploadStore;
}

bool Location::autoIndex()
{
	return _autoindex;
}

void Location::addCgiExt(const std::string &ext)
{
	_cgiExt.push_back(ext);
}