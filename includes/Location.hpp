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
		void				setUrl(const std::string &url);
		void				addMethod(const enums &method);
		void				setRoute(const std::string &route);
		void				setAutoIndex(const bool &autoindex);
		void				setIndex(const std::string &index);
		void				setUploadStore(const std::string &store);
		void				setReturn(const int &code, const std::string &page);
		void				setCgiExt(const std::string &ext);
		bool				isMethodValid(enums method);
		const std::string&	getUrl();
		void				printMethods();
};

#endif
