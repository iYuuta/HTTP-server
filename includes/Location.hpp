# ifndef LOCATION_HPP
# define LOCATION_HPP

# include <vector>
# include <iostream>

#include "utils.hpp"
#include "HttpRequestMethod.hpp"

class Location
{
	private:
		std::string						_url;
		std::vector<HttpRequestMethod>  _methods;
		std::string						_route;
		bool							_autoindex;
		std::string						_index;
		std::string						_uploadStore;
		std::vector<std::string>		_cgiExt;
		std::pair<int, std::string>		_return;
	public:
		Location();
		void				setUrl(const std::string &url);
		void				addMethod(const HttpRequestMethod &method);
		void				addCgiExt(const std::string &ext);
		void				setRoute(const std::string &route);
		void				setAutoIndex(const bool &autoindex);
		void				setIndex(const std::string &index);
		void				setUploadStore(const std::string &store);
		void				setReturn(const int &code, const std::string &page);
		bool				isMethodValid(const HttpRequestMethod &method);
		bool				isRedirect();
		const std::string&	getUrl() const;
		const std::pair<int, std::string>&	getReturn();
		const std::string&	getRoute();
		const std::string&	getIndex();
		const std::string&	getUploadStore();
		const std::vector<std::string>	getExt();
		bool				autoIndex();
		void				printMethods();
};

#endif
