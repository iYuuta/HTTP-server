#include "../../includes/Client.hpp"

bool Client::isTargetValid()
{
	// for (_locationIt = _locations.begin(); _locationIt != _locations.end(); _locationIt++)
	// {
	// 	if (_locationIt->getUrl() == request.getPath())
	// 	{
	// 		std::cout << "valid target\n";
	// 		_locationIt->printMethods();
	// 		return true;
	// 	}
	// }
	std::cout << "invalid target\n";
	return false;
}

bool Client::isMethodValid() const
{
	// if (_locationIt->isMethodValid(request.getMeth()))
	// {
	// 	std::cout << "valid method\n";
	// 	return true;
	// }
	std::cout << "invalid method\n";
	return false;
}

bool Client::isBodySizeValid()
{
	// if (request.getReceivedBytes() > _maxRequestSize)
		return false;
	return true;
}
