#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include "Request.hpp"
#include "utils.hpp"

#define DEF_ERROR "<!DOCTYPE html>\n<html>\n<head><title>Error</title></head>\n<body>\n  <h1>An error occurred</h1>\n  <p>Sorry, something went wrong.</p>\n</body>\n</html>\n"

enum ParseState {
    LOOKING_FOR_START_BOUNDARY,
    PARSING_HEADERS,
    STREAMING_BODY,
    FINISHED
};

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
		std::string							_cgiExt;
		std::string							_cgiFile;
		std::vector<char*>					_envPtr;
		std::vector<std::string>			_env;
		ssize_t								_contentLen;
		int									_errorCode;
		int									_cgiFd;
		int									_cgiPid;
		bool								_cgiRunning;
		bool								_cgiExecuted;
		bool								_responseBuilt;

		std::string 						_errorResponse;
		std::string 						_return;
		std::string							_headers;
		std::string							_statusLine;
		std::string							_bodyLeftover;
		std::ifstream						_body;
		
		bool								_isError;
		bool								_isCgi;
		bool								_isRedirect;
		bool								_errorPageExists;
		enums								_responseState;
		ssize_t								_bytesSent;

		std::string							_boundary;
		std::vector<Multipart>				_multiparts;

		std::vector<std::string>	_cookies;

		void ERROR();
		void GET();
		void POST();
		void DELETE();
		void CGI();
		void REDIRECT();
		void simpleReqsponse();

		void getBody();
		void buildCgiResponse();
		void buildIndex();
		void initCgi();
		void executeCgi();
		bool addCgiHeaders(const std::string& line);

		void monitorCgi();
		void validateUploadPath(const std::string& uploadPath);
		void handleRawUpload(const std::string& uploadPath);
		void handleMultipartUpload(const std::string& uploadPath);
		bool checkTimeOut();

		bool extractBoundary(const std::string& contentTypeHeader, std::string& boundary);
		void parsePartHeaders(const std::string& headerStr, Multipart& part);
		void parseMultipartBody(const std::string& uploadPath);
		
		Response();

	public:
		Response(Request& req, std::map<int, std::string>& error, std::vector<Location>::iterator& location);
		~Response();

		void buildResponse();
		void setErrorCode(int error);
		void isRedirect();
		enums getResponseState() const ;
		bool isResponseBuilt();
		std::string getResponse();

		void	addCookie(const std::string &cookie);
		void	buildCookies();
};

#endif