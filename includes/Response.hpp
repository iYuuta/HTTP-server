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
		std::string 						_status;
		std::string 						_errorResponse;
		std::map<int, std::string>& 		_errorPages;
		size_t								_contentLen;
		std::ifstream						_body;
		std::fstream						_cgiResponse;
		int									_errorCode;
		bool								_isError;
		bool								_isCgi;
		bool								_errorPageExists;
		enums								_responseState;
		size_t								_bytesSent;

		void ERROR();
		void createHeaders();
		void getBody();
		void CGI();

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
		void setErrorCode(int error);
};

#endif