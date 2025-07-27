#include "../../includes/Client.hpp"

bool Client::isTargetValid()
{
	std::vector<Location>& _locations = _server.getLocations();
	std::string path = request.getPath();

	for (_location = _locations.begin(); _location != _locations.end(); _location++) {
		if (_location->getUrl() == path) {
			return true;
		}
	}
	_errorCode = 404;
	return false;
}

bool Client::isMethodValid() {
	if (_location->isMethodValid(request.getMeth())) {
		return true;
	}
	_errorCode = 405;
	return false;
}

bool Client::isBodySizeValid()
{
	if (request.getReceivedBytes() > _server.getMaxRequestSize()) {
		_errorCode = 413;
		return false;
	}
	return true;
}

bool Client::isRequestValid () {
	if (request.getErrorCode())
		return false;
	return isTargetValid() && isMethodValid() && isBodySizeValid();
}