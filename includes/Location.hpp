# ifndef LOCATION_HPP
# define LOCATION_HPP

# include <iostream>
# include <vector>

#include "utils.hpp"

class Location
{
	private:
		std::string						_url;
		std::vector<enums>				_methods;
		std::string						_route;
		bool							_autoindex;
		std::string						_index;
		std::string						_uploadStore;
		std::string						_cgiExt;
		std::pair<int, std::string>		_return;
	public:
		Location();
		void SetUrl(const std::string &url);
		void addMethod(const enums &method);
		void SetRoute(const std::string &route);
		void SetAutoIndex(const bool &autoindex);
		void SetIndex(const std::string &index);
		void SetUploadStore(const std::string &store);
		void SetReturn(const int &code, const std::string &page);
		void SetCgiExt(const std::string &ext);
};

#endif
