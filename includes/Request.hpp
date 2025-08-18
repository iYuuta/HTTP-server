#ifndef REQUEST_HPP 
#define REQUEST_HPP 

# include"utils.hpp"
# include"Location.hpp"

class Request {
	private:
		HttpRequestMethod					_method;
		enums								_parseState;	
		size_t								_contentLen;
		size_t								_receivedBytes;
		int									_errorCode;
		std::string							_buffer;
		bool								_simpleRequest;
		
		std::string							_path;
		std::string							_version;
		std::map<std::string, std::string>	_headers;
		std::string							_queryString;
		std::string							_pathInfo;
		std::string							_bodyFileName;
		std::ofstream						_bodyOut;

		std::map<std::string, std::string>	_cookies;

	public:
		Request();
		~Request();

		void					parseData(const char *data, size_t len);
		void					addRequestLine(const std::string& buff);
		void					addHeaders(std::string buff);
		void					addBody(const std::string& buff, size_t len);
		void					setPath(const std::string& path);
		void					decodeUrl();
		void 					checkForPathInfo(std::vector<Location>::iterator& location);


		bool					isValidRequestLine(const std::string& line);
		const HttpRequestMethod	&getMeth() const ;
		enums					getParseState() const ;
		const std::string&		getPath() const ;
		const std::string&		getVersion() const ;
		size_t					getContentLen()	const ;
		size_t					getReceivedBytes()	const ;
		int						getErrorCode()	const ;
		const std::string		getHeader(const std::string& key) ;
		const std::string&		getFileName();
		std::string&			getQueryStrings();
		bool					isSimpleRequest();
		
		std::map<std::string, std::string>&	getHeaders();

		void parseCookie(const std::string &cookie);
		std::string getCookie(const std::string &key);
};


#endif