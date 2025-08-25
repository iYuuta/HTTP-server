#include "../../includes/Client.hpp"

bool Client::isTargetValid() {
	const std::string& path = request.getPath();
	std::vector<Location>& locations = _server.getLocations();
	std::vector<Location>::iterator bestMatch = locations.end();
	size_t bestLength = 0;

	for (std::vector<Location>::iterator it = locations.begin(); it != locations.end(); ++it) {
		const std::string& locUrl = it->getUrl();
		if (path.compare(0, locUrl.size(), locUrl) == 0) {
			if (locUrl.size() > bestLength) {
				bestLength = locUrl.size();
				bestMatch = it;
			}
		}
	}
	if (bestMatch != locations.end()) {
		_location = bestMatch;
		std::string newPath = path.substr(bestLength);
		if (newPath.empty() || newPath[0] != '/')
			newPath = "/" + newPath;
		if (!normalizePath(newPath, _location->getRoute())){
			_errorCode = 403;
			return false;
		}
		if (!locationExists(_location->getRoute() + newPath)) {
			_errorCode = 404;
			return false;
		}
		request.setPath(newPath);
		if (_location->isRedirect())
			response.isRedirect();
		return true;
	}
	_errorCode = 404;
	return false;
}

bool Client::isMethodValid() {
	if (_location->isMethodValid(request.getMeth()))
		return true;
	_errorCode = 405;
	return false;
}

bool Client::isBodySizeValid() {
	if (_location->isRedirect())
		return true;
	if (request.getContentLen() > (size_t)_server.getMaxRequestSize()) {
		_errorCode = 413;
		return false;
	}
	return true;
}

bool Client::isRequestValid () {
	if (request.getErrorCode() != -1)
		return false;
	return isTargetValid() && isMethodValid() && isBodySizeValid();
}
