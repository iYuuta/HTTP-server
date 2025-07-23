#ifndef CLIENT_HPP
#define CLIENT_HPP

#include "Response.hpp"
#include "Request.hpp"
#include "Location.hpp"

class Client {
	private:
		const int						_fd;
		std::string						_buffer;
		const size_t					_maxRequestSize;
		std::vector<Location>::iterator	_locationIt;
		std::vector<Location>			_locations;
		int								_errorCode;
		bool							_responseDone;
		bool							_requestDone;
		bool							_activeCgi;
		Client();
		
	public:
		Request request;
		Response response;
		Client(std::vector<Location> locations, size_t MaxRequestSize, int fd);
		void getData(char *buffer, size_t len);
		bool					isRequestDone();
		bool					isResponseDone();
		~Client();
};

#endif