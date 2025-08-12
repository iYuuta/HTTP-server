#include "../../../includes/Response.hpp"

Response::~Response() {}

Response::Response(Request& req, std::map<int, std::string>& error, std::vector<Location>::iterator& location):
_request(req),
_location(location),
_errorPages(error),
_errorCode(200),
_isError(false),
_isCgi(false),
_isRedirect(false),
_errorPageExists(true),
_responseState(STATUSLINE_HEADERS),
_bytesSent(0) {
	_errorResponse.clear();
	_statusLine_Headers.clear();
	_contentType.clear();
	_env.clear();
	_status.clear();
	_cgiExt.clear();
	_envPtr.clear();
	_return.clear();
}

void Response::validateUploadPath(const std::string& uploadPath) {
	if (uploadPath.empty()) {
		_errorCode = 403;
		throw std::runtime_error("File uploads are not configured for this location.");
	}

	struct stat dirStat;
	if (stat(uploadPath.c_str(), &dirStat) != 0) {
		if (mkdir(uploadPath.c_str(), 0755) != 0) {
			_errorCode = 500;
			throw std::runtime_error("Failed to create upload directory.");
		}
	} else if (!S_ISDIR(dirStat.st_mode)) {
		_errorCode = 500;
		throw std::runtime_error("Upload path exists but is not a directory.");
	}
}

void Response::handleRawUpload(const std::string& uploadPath) {
	std::string requestPath = _request.getPath();
	std::string filename = requestPath.substr(requestPath.find_last_of('/') + 1);
	if (filename.empty() || filename == "/") {
		_errorCode = 400;
		throw std::runtime_error("Cannot upload body to a directory");
	}
	std::string filePath = uploadPath + "/" + filename;

	std::ofstream newFile(filePath.c_str(), std::ios::out | std::ios::binary);
	if (!newFile.is_open()) {
		_errorCode = 500;
		throw std::runtime_error("Failed to create the file on the server.");
	}

	std::ifstream& bodyFile = const_cast<std::ifstream&>(_request.getBodyFile());
	if (!bodyFile.is_open()) {
		_errorCode = 500;
		throw std::runtime_error("Failed to open request body file.");
	}

	newFile << bodyFile.rdbuf();
	bodyFile.close();
	newFile.close();

	_statusLine_Headers.append("HTTP/1.0 204 No Content\r\n");

	for (size_t i = 0; i < _cookies.size(); i++)
		 _statusLine_Headers.append("Set-Cookie: " + _cookies[i] + "\r\n");

	_statusLine_Headers.append("Connection: close\r\n\r\n");
}

void Response::handleMultipartUpload(const std::string& uploadPath) {
		
	parseMultipartBody();

	for (size_t i = 0; i < _multiparts.size(); i++) {
		const Multipart& part = _multiparts[i];
		if (part.isFile && !part.contentDispositionFilename.empty()) {
			std::string finalFilePath = uploadPath + "/" + part.contentDispositionFilename;
			
			std::ifstream src(part.tempFilePath.c_str(), std::ios::binary);
			std::ofstream dst(finalFilePath.c_str(), std::ios::binary);

			if (!src.is_open() || !dst.is_open()) {
				_errorCode = 500;
				unlink(part.tempFilePath.c_str());
				throw std::runtime_error("Failed to open streams for file copy.");
			}
			
			dst << src.rdbuf();
			src.close();
			dst.close();
			
			unlink(part.tempFilePath.c_str());
		} else if (part.isFile) {
			unlink(part.tempFilePath.c_str());
		}
	}

	_statusLine_Headers.append("HTTP/1.0 201 Created\r\n");

	for (size_t i = 0; i < _cookies.size(); i++)
		 _statusLine_Headers.append("Set-Cookie: " + _cookies[i] + "\r\n");

	_statusLine_Headers.append("Connection: close\r\n\r\n");
}

void Response::initCgi() {
	_cgiExt = getExtension(_request.getPath());
	if (_cgiExt.empty()) {
		_errorCode = 501;
		throw (std::string) "unsupported cgi extention";
	}
	_env.clear();

	_env.push_back("REQUEST_METHOD=" + methodToStr(_request.getMeth()));
	_env.push_back("CONTENT_LENGTH=" + intToString(_request.getContentLen()));
	_env.push_back("CONTENT_TYPE=" + _request.getHeader("Content-Type"));
	_env.push_back("SCRIPT_NAME=" + _location->getRoute() + _request.getPath());
	_env.push_back("SERVER_PROTOCOL=HTTP/1.0");
	_env.push_back("GATEWAY_INTERFACE=CGI/1.1");
	_env.push_back("SERVER_SOFTWARE=Webserv/1.0");
	_env.push_back("HTTP_HOST=" + _request.getHeader("Host"));

	if (!_request.getQueryStrings().empty())
		_env.push_back("QUERY_STRING=" + _request.getQueryStrings());
	for (size_t i = 0; i < _env.size(); ++i)
		_envPtr.push_back(const_cast<char*>(_env[i].c_str()));
	_envPtr.push_back(NULL);
}

void Response::executeCgi() {
	int fd = 0;

	_cgiFile = generateRandomName();
	_cgiFd = open(_cgiFile.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0644);
	if (_cgiFd < 0) {
		_errorCode = 500;
		throw (std::string) "failed to open a file";
	}
	if (_request.getContentLen() > 0) {
		fd = open(_request.getFileName().c_str(), O_RDONLY);
		if (fd < 0) {
			unlink(_cgiFile.c_str());
			close(_cgiFd);
			_errorCode = 500;
			throw (std::string) "failed to open a file";
		}
	}
	int pid = fork();
	if (pid < 0) {
		if (fd != 0) {
			unlink(_request.getFileName().c_str());
			close(fd);
		}
		_errorCode = 500;
		unlink(_cgiFile.c_str());
		close(_cgiFd);
		throw (std::string) "fork failed";
	}
	if (pid == 0) {
		if (chdir(_location->getRoute().c_str()) < 0) {
			if (fd != 0) {
				unlink(_request.getFileName().c_str());
				close(fd);
			}
			_errorCode = 500;
			unlink(_cgiFile.c_str());
			close(_cgiFd);
			exit(500);
		}
		if (fd != 0) {
			dup2(fd, STDIN_FILENO);
			close(fd);
		}
		dup2(_cgiFd, STDOUT_FILENO);
		close(_cgiFd);
		std::string file = _request.getPath();
		file = file.erase(0, 1);
		char *argv[] = {const_cast<char*>(_cgiExt.c_str()), const_cast<char*>(file.c_str()), NULL};
		execve(_cgiExt.c_str(), argv, _envPtr.data());
		exit(500);
	}
	else {
		int status;
		waitpid(pid, &status, 0);
		unlink(_request.getFileName().c_str());
		close(fd);
		if (status == 500) {
			_errorCode = 500;
			unlink(_cgiFile.c_str());
			close(_cgiFd);
			throw (std::string) "child process terminated with a failure";
		}
		close(_cgiFd);
		_cgiFd = open(_cgiFile.c_str(), O_RDONLY);
		if (_cgiFd < 0) {
   			_errorCode = 500;
			throw (std::string) "failed to open a file";
		}
		unlink(_cgiFile.c_str());
	}
}

bool Response::addCgiHeaders(const std::string& line) {
	size_t pos = line.find(":");

	if (pos == std::string::npos) {
		if (line.empty() || (line[0] != ' ' && line[0] != '\t'))
			return false;		
		if (_statusLine_Headers.empty())
			return false;		
		_statusLine_Headers += " " + trim(line) + "\r\n";
		return true;
	}
	std::string key = line.substr(0, pos);

	if (!isKeyValid(key))
		return false;	
	std::string value = trim(line.substr(pos + 1));

	_statusLine_Headers += key + ": " + value + "\r\n";
	if (key == "Content-Length") {
		if (value.empty())
			return false;		
		char* endptr = NULL;
		unsigned long long len = std::strtoull(value.c_str(), &endptr, 10);
		if (endptr == value.c_str() || *endptr != '\0')
			return false;		
		_contentLen = static_cast<size_t>(len);
	}
	return true;
}

void Response::CGI() {
	try {
		initCgi();
		executeCgi();
	}
	catch (std::string err) {
		std::cerr << "Error: " << err << std::endl;
		_isError = true;
		ERROR();
		return ;
	}
	_statusLine_Headers.append("HTTP/1.0 200 OK\r\n");
	char		buffer[BUFFER_SIZE];
	ssize_t		bytes;
	std::string strbuff;
	std::string line;
	std::string fileName;
	
	bool body = false;
	fileName = generateRandomName();
	_outBody.open(fileName.c_str());
	if (!_outBody.is_open()) {
		std::cerr << "Error: failed to open a file" << std::endl;
		_isError = true;
		_errorCode = 500;
		ERROR();
		close(_cgiFd);
		return ;
	}
	while ((bytes = read(_cgiFd, buffer, BUFFER_SIZE)) > 0) {
		strbuff.append(buffer, bytes);
		while (!strbuff.empty()) {
			if (!body) {
				size_t pos = strbuff.find("\r\n");
				if (pos == std::string::npos)
					break ;
				line = strbuff.substr(0, pos);
				strbuff.erase(0, pos + 2);
				if (line.empty()) {
					_statusLine_Headers.append("\r\n");
					body = true;
					continue;
				}
				else if (!addCgiHeaders(line)) {
					std::cerr << "Error: Bad Gateway" << std::endl;
					unlink(fileName.c_str());
					_outBody.close();
					_isError = true;
					_errorCode = 502;
					ERROR();
					close(_cgiFd);
					return ;
				}
			}
			else {
				_outBody.write(strbuff.c_str(), strbuff.size());
				strbuff.clear();
			}
		}
	}
	_outBody.flush();
	if (bytes < 0 || (bytes == 0 && !body)) {
		std::cerr << "Error: Internal Server Error" << std::endl;
		unlink(fileName.c_str());
		_outBody.close(); 
		_errorCode = 500;
		_isError = true;
		ERROR();
		close(_cgiFd);
		return ;
	}
	else {
		if (_contentLen == 0) {
			struct stat fileStat;
			stat(fileName.c_str(), &fileStat);
			_contentLen = fileStat.st_size;
		}
		_outBody.close();
		_body.open(fileName.c_str());
		if (!_body.is_open()) {
			std::cerr << "Error: failed to open a file" << std::endl;
			unlink(fileName.c_str());
			_outBody.close();
			_isError = true;
			_errorCode = 500;
			ERROR();
			close(_cgiFd);
			return ;
		}
	}
	unlink(fileName.c_str());
	close(_cgiFd);
	return ;
}

void Response::ERROR() {
	std::string errorFile = _errorPages[_errorCode];
	struct stat fileStat;

	if (_body.is_open())
		_body.close();
	if (errorFile.length() == 0)
		_errorPageExists = false;
	else {
		if (stat(errorFile.c_str(), &fileStat) == 0)
			_contentLen = fileStat.st_size;
		_body.open(errorFile.c_str(), std::ios::in | std::ios::binary);
		if (!_body.is_open()) {
			_errorPageExists = false;
			_errorCode = 500;
		}
	}
	switch (_errorCode) {
		case 400 :
			_errorResponse.append("HTTP/1.0 400 Bad Request\r\n");
			break;
		case 404 :
			_errorResponse.append("HTTP/1.0 404 Not Found\r\n");
			break;
		case 403 :
			_errorResponse.append("HTTP/1.0 403 Forbidden\r\n");
			break;
		case 405 :
			_errorResponse.append("HTTP/1.0 405 Method Not Allowed\r\n");
			break;
		case 413 :
			_errorResponse.append("HTTP/1.0 413 Payload Too Large\r\n");
			break;
		case 501 :
			_errorResponse.append("HTTP/1.0 501 Not Implemented\r\n");
			break;
		case 409:
			_errorResponse.append("HTTP/1.0 409 Conflict\r\n");
			break;
		default:
			_errorResponse.append("HTTP/1.0 500 Internal Server Error\r\n");
			break;
	}
	if (!_errorPageExists) {
		_errorResponse.append("Content-Type: text/html\r\nContent-Length: 147\r\nConnection: Close\r\n\r\n");
		_errorResponse.append(DEF_ERROR);
		return;
	}
	_errorResponse.append("Content-Type: text/html\r\n");
	_errorResponse.append("Content-Length: " + intToString(_contentLen) + "\r\n");

	for (size_t i = 0; i < _cookies.size(); i++)
		 _statusLine_Headers.append("Set-Cookie: " + _cookies[i] + "\r\n");

	_errorResponse.append("Connection: Close\r\n\r\n");
	std::ostringstream ss;
	ss << _body.rdbuf();
	_errorResponse += ss.str();
}

void Response::buildIndex() {
	DIR *dir = opendir((_location->getRoute() + _request.getPath()).c_str());
	std::string fileName = generateRandomName();
	std::string path;
	std::ofstream out;

	if (_request.getPath() == "/")
		path = _location->getUrl();
	else
		path = _request.getPath();
	out.open(fileName.c_str());
	if (!out.is_open()) {
		_errorCode = 500;
		throw (std::string) "Open failed";
		return ;
	}
	out << "<!DOCTYPE html>\n<html>\n<head>\n<title>Index of " << path << "</title>\n"
		<< "<meta charset=\"utf-8\">\n<style>\n"
		<< "body { font-family: Arial, sans-serif; }\n"
		<< "table { border-collapse: collapse; width: 100%; }\n"
		<< "th, td { padding: 8px; border-bottom: 1px solid #ddd; }\n"
		<< "a { text-decoration: none; color: #0366d6; }\n"
		<< "a:hover { text-decoration: underline; }\n"
		<< "</style>\n</head>\n<body>\n"
		<< "<h1>Index of " <<  path << "</h1>\n<hr>\n<table>\n"
		<< "<tr><th>Name</th></tr>\n";
	dirent *entry;
	while ((entry = readdir(dir)) != NULL) {
		std::string name = entry->d_name;

		if (name == "." || name == "..")
			continue;
		out << "<tr><td onclick=\"window.location.href+=\'/" << name <<"\' \">" << name << "</td></tr>\n";
	}
	out << "</table>\n<hr>\n</body>\n</html>\n";
	closedir(dir);
	out.close();
	_body.open(fileName.c_str(), std::ios::in | std::ios::binary);
	if (!_body.is_open()) {
		_errorCode = 500;
		throw (std::string) "failed to open a file";
	}
	struct stat fileStat;
	stat(fileName.c_str(), &fileStat);
	_contentLen = fileStat.st_size;
	_contentType = "text/html";
	unlink(fileName.c_str());
}

void Response::getBody() {

	struct stat fileStat;
	std::string fileName = _location->getRoute() + _request.getPath();

	if (isDirectory(fileName)) {
		if (_location->autoIndex()) {
			if (!_location->getIndex().empty())
				fileName += _location->getIndex();
			else {
				try {
					buildIndex();
					return ;
				}
				catch (std::string err) {
					throw err;
				}
			}
		}
		else {
			_errorCode = 404;
			throw (std::string) "AutoIndex off and no index file";
		}
	}
	if (stat(fileName.c_str(), &fileStat) == 0)
		_contentLen = fileStat.st_size;
	else {
		std::cout << fileName << std::endl;
		_errorCode = 404;
		throw (std::string) "file not found";
	}
	_body.open(fileName.c_str(), std::ios::in | std::ios::binary);
	if (!_body.is_open()) {
		_errorCode = 500;
		throw (std::string) "failed to open a file";
	}
	_contentType = getContentType(fileName);
}

void Response::GET() {
	if (_errorCode != 200 && _errorCode != -1) {
		ERROR();
		return ;
	}
	try {
		getBody();
		_statusLine_Headers.append("HTTP/1.0 " + intToString(_errorCode) + " OK\r\n");
		_statusLine_Headers.append("Content-Type: " + _contentType + "\r\n");
		_statusLine_Headers.append("Content-Length: " + intToString(_contentLen) + "\r\n");

		for (size_t i = 0; i < _cookies.size(); i++)
			_statusLine_Headers.append("Set-Cookie: " + _cookies[i] + "\r\n");

		_statusLine_Headers.append("Connection: Close\r\n\r\n");
	}
	catch (std::string error) {
		_isError = true;
		std::cerr << error << std::endl;
		ERROR();
	}
}

void Response::POST() {
	if (_errorCode != 200 && _errorCode != -1) {
		_isError = true;
		ERROR();
		return;
	}
	try {
		std::string uploadPath = _location->getUploadStore();
		validateUploadPath(uploadPath);
		
		std::string contentType = _request.getHeader("Content-Type");
		if (contentType.find("multipart/form-data") != std::string::npos)
			handleMultipartUpload(uploadPath);
		else
			handleRawUpload(uploadPath);
		_contentLen = 0;
	} catch (const std::exception& e) {
		_isError = true;
		ERROR();
		std::cerr << "POST Error: " << e.what() << std::endl;
	}
}

void Response::DELETE() {

	if (_errorCode != 200 && _errorCode != -1) {
		_isError = true;
		ERROR();
		return ;
	}
 	try {
		struct stat fileStat;
		std::string fileName = _location->getRoute() + _request.getPath();

		if (stat(fileName.c_str(), &fileStat) != 0) {
			_errorCode = 404;
			throw (std::string) "file not found";
		}

		if (!S_ISREG(fileStat.st_mode)) {
			_errorCode = 403;
			throw (std::string) "not a regular file";
		}

		if (unlink(fileName.c_str()) != 0) {
			_errorCode = 403;
			throw (std::string) "failed to delete file";
		}

		_statusLine_Headers.clear();
		_statusLine_Headers.append("HTTP/1.0 204 No Content\r\n");

		for (size_t i = 0; i < _cookies.size(); i++)
		 _statusLine_Headers.append("Set-Cookie: " + _cookies[i] + "\r\n");

		_statusLine_Headers.append("Connection: Close\r\n\r\n");
		_contentLen = 0;

	}
	catch (std::string error) {
		_isError = true;
		ERROR();
		std::cerr << error << std::endl;
	}
}

void Response::REDIRECT() {
	_return.append("HTTP/1.0 " + intToString(_location->getReturn().first) + " Moved Permanently\r\n" + "Location: " + _location->getReturn().second + "\r\n");
}

void Response::simpleReqsponse() {
	if (_request.getMeth() == Get) {
		try {
			getBody();
		}
		catch (std::string err) {
			std::cerr << "Error: " << err << std::endl;
			_isError = true;
			ERROR();
		}
	}
	else {
		_errorCode = 501;
		_isError = true;
		ERROR();
	}
}

void Response::buildCookies() 
{
	std::string sessionId = _request.getCookie("session-id");
	std::cout << "session id : " << sessionId << std::endl;
	if (sessionId.empty())
	{
		std::cout << "First time ?" << std::endl;

		std::stringstream ss;
		ss << "user-" << rand();
		std::string newSessionId = ss.str();
		
		std::string cookie = "session-id=" + newSessionId + "; Path=/";

		addCookie(cookie);
	}
	else
		std::cout << "Hello " << sessionId << std::endl;
}

void Response::buildResponse() {
	
	if (_errorCode != 200) {
		_isError = true;
		ERROR();
		return ;
	}

	buildCookies();

	if (isExtension(_request.getPath(), _location->getExt())) {
		if (_request.isSimpleRequest()) {
			_isError = true;
			_errorCode = 400;
			ERROR();
			return ;
		}
		CGI();
		return ;
	}
	else if (_isRedirect) {
		REDIRECT();
		return ;
	}
	else if (_request.isSimpleRequest()) {
		simpleReqsponse();
		return ;
	}
	switch (_request.getMeth()) {
		case Get:
			GET();
			break;
		case Post:
			POST();
			break;
		case Delete:
			DELETE();
			break;
		default:
			_isError = true;
			ERROR();
			break;
	}
}

std::string Response::getResponse() {
	if (_isError) {
		if (_body.is_open())
			_body.close();
		_responseState = DONE;
		std::cout << _errorResponse << std::endl;
		return _errorResponse;
	}
	if (_request.isSimpleRequest()) {
		if (_contentLen == 0) {
			_responseState = DONE;
			return "";
		}
		if (_bytesSent < _contentLen) {
			char buffer[BUFFER_SIZE];
			size_t toRead = std::min(static_cast<size_t>(BUFFER_SIZE), _contentLen - _bytesSent);

			_body.read(buffer, toRead);
			size_t actuallyRead = _body.gcount();
			_bytesSent += actuallyRead;

			if (_bytesSent >= _contentLen) {
				_body.close();
				_responseState = DONE;
			}
			return std::string(buffer, actuallyRead);
		}
		_responseState = DONE;
		return "";
	}
	if (_isRedirect) {
		_responseState = DONE;
		return _return;
	}
	switch (_responseState) {
		case STATUSLINE_HEADERS: {
			_responseState = BODY;
			return _statusLine_Headers;
		}
		case BODY: {
			if (_contentLen == 0) {
				_responseState = DONE;
				return "";
			}
			if (_bytesSent < _contentLen) {
				char buffer[BUFFER_SIZE];
				size_t toRead = std::min(static_cast<size_t>(BUFFER_SIZE), _contentLen - _bytesSent);

				_body.read(buffer, toRead);
				size_t actuallyRead = _body.gcount();
				_bytesSent += actuallyRead;

				if (_bytesSent >= _contentLen) {
					_body.close();
					_responseState = DONE;
				}
				return std::string(buffer, actuallyRead);
			}
			_responseState = DONE;
			return "";
		}
		default:
			_responseState = DONE;
			return "";
	}
}

void Response::setErrorCode(int error) {
	_errorCode = error;
}

void Response::isRedirect() {
	_isRedirect = true;
}

enums Response::getResponseState() const {
	return _responseState;
}

void Response::addCookie(const std::string &cookie)
{
	_cookies.push_back(cookie);
}
