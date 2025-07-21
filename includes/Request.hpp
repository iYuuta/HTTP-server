#ifndef REQUEST_HPP 
#define REQUEST_HPP 

#include <map>
#include <string>
#include <exception>
#include <sys/socket.h>

#include"utils.hpp"

#define BUFFER_SIZE 4096

class Request {
	private:
		int									_method;
		int									_parse_state;
		std::string							_path;
		std::string							_version;
		std::map<std::string, std::string>	_headers;
		int									_bodyFd;
		size_t								_content_len;
		size_t								_received_bytes;
	public:
		Request();
		~Request();

		void addRequestLine(std::string buff);
		void addHeaders(std::string buff);
		void addBody(std::string buff);
};

#endif