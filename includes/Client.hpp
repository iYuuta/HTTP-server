#ifndef CLIENT_HPP
#define CLIENT_HPP

#include "Respond.hpp"
#include "Request.hpp"

class Client {
	private:
		int						_fd;
		std::string				_buffer;
		size_t					_MaxRequestSize;
		std::vector<Location>*   _locations;
		int						_error_code;
		bool					_response_ready;
		bool					_ActiveCgi;

	public:
		Request request;
		Respond response;
		Client();
		~Client();
};

#endif