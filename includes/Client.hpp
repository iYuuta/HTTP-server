#ifndef CLIENT_HPP
#define CLIENT_HPP

#include "Response.hpp"
#include "Request.hpp"
#include "Size.hpp"
#include "Location.hpp"
#include <unistd.h>

class Client {

	private:
		int								_fd;
		std::string						_buffer;
		int								_errorCode;
		bool							_responseDone;
		bool							_requestDone;
		bool							_activeCgi;
		bool							isTargetValid();
		bool							isMethodValid() const;
		bool							isBodySizeValid();
	public:
		Request request;
		Response response;
		Client(const int &fd);
		void readData();
		bool					isRequestDone();
		bool					isResponseDone();
		~Client();

};

#endif