#include "../../../includes/Request.hpp"

bool Request::isTargetValid()
{
	for (_locationIter = _locations.begin(); _locationIter != _locations.end(); _locationIter++)
	{
		if (_locationIter->getUrl() == _path)
		{
			std::cout << "valid target\n";
			_locationIter->printMethods();
			return true;
		}
	}
	std::cout << "invalid target\n";
	return false;
}

bool Request::isMethodValid() const
{
	if (_locationIter->isMethodValid(_method))
	{
		std::cout << "valid method\n";
		return true;
	}
	std::cout << "invalid method\n";
	return false;
}

bool Request::isBodySizeValid()
{
	if (_receivedBytes > _maxBodySize)
		return false;
	return true;
}
