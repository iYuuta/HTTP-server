#ifndef CLIENT_HPP
#define CLIENT_HPP

#include "Response.hpp"
#include "Request.hpp"
#include "Size.hpp"
#include "Location.hpp"
#include <unistd.h>

class Client {
	private:
		const int						_fd;
		std::string						_buffer;
		const Size						_maxRequestSize;
		std::vector<Location>::iterator	_locationIt;
		std::vector<Location>			_locations;
		int								_errorCode;
		bool							_responseDone;
		bool							_requestDone;
		bool							_activeCgi;
	public:
		Request request;
		Response response;
		Client(const std::vector<Location> &locations, const Size &maxRequestSize, const int &fd);
		void readData();
		bool					isRequestDone();
		bool					isResponseDone();
		~Client();
};

#endif