#ifndef CLIENT_HPP
#define CLIENT_HPP

#include "Response.hpp"
#include "Request.hpp"

class Location;

class Client {
	private:
		const int				_fd;
		std::string				_buffer;
		const size_t			_MaxRequestSize;
		std::vector<Location>*	_locations;
		int						_error_code;
		bool					_response_done;
		bool					_request_done;
		bool					_ActiveCgi;
		Client();
		
	public:
		Request request;
		Response response;
		Client(std::vector<Location>* locations, size_t MaxRequestSize, int fd);
		void GetData(char *buffer, size_t len);
		bool					IsRequestDone();
		bool					IsResponseDone();
		~Client();
};

#endif