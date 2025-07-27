#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include "Request.hpp"
#include "utils.hpp"

#define DEF_ERROR "<!DOCTYPE html>\n<html>\n<head><title>Error</title></head>\n<body>\n  <h1>An error occurred</h1>\n  <p>Sorry, something went wrong.</p>\n</body>\n</html>\n"

class Response {
	private:
		Request&							_request;
		std::vector<Location>::iterator&	_location;
		std::string							_statusLine_Headers;
		std::string 						_contentType;
		std::map<int, std::string>& 		_errorPages;
		size_t								_contentLen;
		std::ifstream						_body;
		int									_errorCode;
		bool								_done;
		bool								_ErrorPageExists;
		bool								_readBody;
		enums								_responseState;
		size_t								_bytesSend;

		void ERROR();
		void createStatusLine();
		void createHeaders();
		void getBody();

		void GET();
		void POST();
		void DELETE();

		Response();

		public:
		Response(Request& req, std::map<int, std::string>& error, std::vector<Location>::iterator& location);
		~Response();
		std::string getResponse();
		void buildResponse();
		enums getResponseState() const ;
		bool isResponseDone() const ;
};

#endif