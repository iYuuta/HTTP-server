#ifndef REQUEST_HPP 
#define REQUEST_HPP 

#include <map>
#include <string>
#include <exception>
#include <sys/socket.h>
#include <fcntl.h>

#include"utils.hpp"

#define BUFFER_SIZE 4096

class Request {
	private:
		enums								_method;
		int									_fd;
		enums								_parse_state;
		std::string							_buffer;
		std::string							_path;
		std::string							_version;
		std::map<std::string, std::string>	_headers;
		std::string							_body_file;
		std::ofstream						_body;
		std::ifstream						_body_in;
		size_t								_content_len;
		size_t								_received_bytes;
	public:
		Request();
		Request(int fd);
		~Request();

		void					ParseData(const char *data, size_t len);
		void					AddRequestLine(std::string buff);
		void					AddHeaders(std::string buff);
		void					AddBody(const std::string& buff, size_t len);
		enums					GetMeth() const ;
		enums					GetParseState() const ;
		const std::string&		GetPath() const ;
		const std::string&		GetVersion() const ;
		size_t					GetContentLen()	const ;
		size_t					GetReceivedBytes()	const ;
		const std::string		GetHeader(const std::string& key) ;
		const std::ifstream&	GetBodyFile();
		
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

	int									GetFd() const ;
	std::map<std::string, std::string>	GetHeaders() const ;

};

#endif