#ifndef REQUEST_HPP 
#define REQUEST_HPP 

#include <map>
#include <string>
#include <exception>
#include <fstream>

#include"utils.hpp"
#include"Location.hpp"

#define BUFFER_SIZE 4096

class Request {
	private:
		HttpRequestMethod					_method;
		enums								_parseState;
		size_t								_maxBodySize;
		size_t								_contentLen;
		size_t								_receivedBytes;
		std::string							_buffer;
		std::string							_path;
		std::string							_version;
		std::map<std::string, std::string>	_headers;
		std::string							_bodyFileName;
		std::ofstream						_bodyOut;
		std::ifstream						_bodyIn;
		std::vector<Location>::iterator 	_locationIter;
		std::vector<Location>			 	_locations;
	public:
		Request();
		Request(size_t maxBody, std::vector<Location>& it);
		~Request();

		void					parseData(const char *data, size_t len);
		void					addRequestLine(std::string buff);
		void					addHeaders(std::string buff);
		void					addBody(const std::string& buff, size_t len);
		const HttpRequestMethod	&getMeth() const ;
		enums					getParseState() const ;
		const std::string&		getPath() const ;
		const std::string&		getVersion() const ;
		size_t					getContentLen()	const ;
		size_t					getReceivedBytes()	const ;
		const std::string		getHeader(const std::string& key) ;
		const std::ifstream&	getBodyFile();
		bool					isTargetValid();
		bool					isMethodValid() const;
		bool					isBodySizeValid();
		
	class InvalidRequestLine : public std::exception {
		public :
			const char *what() const throw() ;
	};
	class InvalidHeader : public std::exception {
		public :
			const char *what() const throw() ;
	};
	class OpenFailed : public std::exception {
		public :
			const char *what() const throw() ;
	};
	class WriteError : public std::exception {
		public :
			const char *what() const throw() ;
	};

	std::map<std::string, std::string>	getHeaders() const ;

};

std::string trim(const std::string& s);
std::string generateRandomName();

#endif