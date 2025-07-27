#include "../../includes/Client.hpp"

bool Client::isTargetValid() {
    std::vector<Location>& _locations = _server.getLocations();
    std::string path = request.getPath();
    std::vector<Location>::iterator bestMatch = _locations.end();
    size_t bestLength = 0;

    for (_location = _locations.begin(); _location != _locations.end(); ++_location) {
    	const std::string& locUrl = _location->getUrl();
		if (path == "/" && locUrl == "/") {
			bestLength = locUrl.length();
            bestMatch = _location;
		}
    	if (path != "/" && path.compare(0, locUrl.length(), locUrl) == 0) {
    	    if (locUrl.length() > bestLength) {
	            bestLength = locUrl.length();
                bestMatch = _location;
    		}
    	}
	}
    if (bestMatch != _locations.end()) {
        _location = bestMatch;
        std::string newPath = path.substr(bestLength);
        if (newPath.empty())
    		newPath = "/";
		else if (newPath[0] != '/')
    		newPath = "/" + newPath;
        request.setPath(newPath);
        return true;
    }
    _errorCode = 404;
    return false;
}

bool Client::isMethodValid() {
	if (_location->isMethodValid(request.getMeth())) {
		std::cout << _location->getUrl() << " -> method valid\n";
		return true;
	}
	_errorCode = 405;
	return false;
}

bool Client::isBodySizeValid() {
	if (request.getReceivedBytes() > _server.getMaxRequestSize()) {
		_errorCode = 413;
		return false;
	}
	return true;
}

bool Client::isRequestValid () {
	// if (request.getErrorCode())
	// 	return false;
	return isTargetValid() && isMethodValid(); //&& isBodySizeValid();
}