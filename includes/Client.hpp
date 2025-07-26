#ifndef CLIENT_HPP
#define CLIENT_HPP

#include "Response.hpp"
#include "Request.hpp"
#include "Server.hpp"
#include <unistd.h>

#include "Server.hpp"

class Server;

class Client {

	private:
		Server							&_server;
		std::vector<Location>::iterator	_location;
		int								_fd;
		std::string						_buffer;
		int								_errorCode;
		bool							_responseDone;
		bool							_requestDone;
		bool							_activeCgi;
		bool							isTargetValid();
		bool							isMethodValid() const;
		bool							isBodySizeValid();
		bool							isRequestValid();
	public:
		Request request;
		Response response;
		Client(const int& fd, Server& server);
		void readData();
		bool					isRequestDone();
		bool					isResponseDone();
		~Client();

};

#endif