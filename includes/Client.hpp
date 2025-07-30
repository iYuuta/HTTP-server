#ifndef CLIENT_HPP
#define CLIENT_HPP

#include "Response.hpp"
#include "Request.hpp"
#include "Server.hpp"
#include <unistd.h>

#include "Server.hpp"

class Server;
class Response;

class Client {

	private:
		Server							&_server;
		std::vector<Location>::iterator	_location;
		std::map<int, std::string>		&_errorPages;
		int								_fd;
		std::string						_buffer;
		int								_errorCode;
		bool							_responseDone;
		bool							_requestDone;
		bool							isTargetValid();
		bool							isMethodValid();
		bool							isBodySizeValid();
		bool							isRequestValid();
		Request request;
		Response response;
	public:
		Client(const int& fd, Server& server, std::map<int, std::string>& errorp);
		void parseRequest();
		void createResponse();
		void writeData();
		bool isRequestDone();
		bool isResponseDone();
		bool isFinished();
		~Client();

};

#endif