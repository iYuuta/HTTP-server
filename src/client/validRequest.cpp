#include "../../includes/Client.hpp"

bool Client::isTargetValid()
{
	std::vector<Location>& _locations = _server.getLocations();
	for (_location = _locations.begin(); _location != _locations.end(); _location++)
	{
		if (_location->getUrl() == request.getPath())
		{
			std::cout << "valid target\n";
			return true;
		}
	}
	std::cout << "invalid target\n";
	return false;
}

bool Client::isMethodValid() const
{
	if (_location->isMethodValid(request.getMeth()))
	{
		std::cout << "valid method\n";
		return true;
	}
	std::cout << "invalid method\n";
	return false;
}

bool Client::isBodySizeValid()
{
	if (request.getReceivedBytes() > _server.getMaxRequestSize())
		return false;
	return true;
}

bool Client::isRequestValid () {
	return isTargetValid() && isMethodValid() && isBodySizeValid();
}