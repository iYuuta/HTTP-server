# include "../../includes/Config.hpp"
# include <fstream>
# include <string.h>

Config::Config(const std::string& input) : _errorCode(0)
{
	parseConfig(input);
}

void Config::parseConfig(const std::string& input)
{
	std::ifstream conf(input.c_str());

	if (conf.fail())
	{
		std::cerr << "Error: " << strerror(errno) << ": " << input << std::endl;
		_errorCode = 2;
		return;
	}
	if (!parseServers(conf, *this))
		_errorCode = 3;
	conf.close();
}

void Config::addServer(const Server& server)
{
	_servers.push_back(server);
}

std::vector<Server>& Config::getServers()
{
	return (_servers);
}

const int &Config::getErrorCode() const
{
	return (_errorCode);
}

bool Config::fail() const
{
	return (_errorCode != 0);
}
