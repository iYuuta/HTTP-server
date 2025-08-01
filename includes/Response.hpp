#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include "Request.hpp"
#include "utils.hpp"

#define DEF_ERROR "<!DOCTYPE html>\n<html>\n<head><title>Error</title></head>\n<body>\n  <h1>An error occurred</h1>\n  <p>Sorry, something went wrong.</p>\n</body>\n</html>\n"

struct Multipart {
    std::map<std::string, std::string> headers;
    std::string contentDispositionName;
    std::string contentDispositionFilename;
    std::string contentType;
    std::string contentBody;
    std::string tempFilePath;
    bool isFile;

    Multipart() : isFile(false) {}
};

class Response {
	private:
		Request&							_request;
		std::vector<Location>::iterator&	_location;
		std::map<int, std::string>& 		_errorPages;

		std::string 						_contentType;
		std::string 						_status;
		size_t								_contentLen;
		int									_errorCode;
		std::vector<std::string>			_env;
		std::string							_cgiExt;
		std::string							_cgiFile;
		std::vector<char*>					_envPtr;

		std::string 						_errorResponse;
		std::string 						_return;
		std::string							_statusLine_Headers;
		std::ifstream						_body;
		int									_cgiFd;
		
		bool								_isError;
		bool								_isCgi;
		bool								_isRedirect;
		bool								_errorPageExists;
		enums								_responseState;
		size_t								_bytesSent;

		std::string							_boundary;
		std::vector<Multipart>				_multiparts;

		void ERROR();
		void GET();
		void POST();
		void DELETE();
		void CGI();
		void REDIRECT();

		void getBody();
		void initCgi();

		void parseMultipartBody();
		bool extractBoundary(const std::string& contentTypeHeader, std::string& boundary);
		void parsePartHeaders(const std::string& headerStr, Multipart& part);
		void processPartBody(Multipart& part, const std::string& bodyContent);
		bool readFileToString(const std::string& filePath, std::string& content);

		Response();

	public:
		Response(Request& req, std::map<int, std::string>& error, std::vector<Location>::iterator& location);
		~Response();

		void buildResponse();
		void setErrorCode(int error);
		void isRedirect();
		enums getResponseState() const ;
		std::string getResponse();
};

#endif