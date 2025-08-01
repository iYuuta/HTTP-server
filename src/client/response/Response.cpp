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
	_errorResponse.append("Connection: Close\r\n\r\n");
	std::ostringstream ss;
	ss << _body.rdbuf();
	_errorResponse += ss.str();
}

void Response::getBody() {

	struct stat fileStat;
	std::string fileName = _location->getRoute() + _request.getPath();

	if (isDirectory(fileName)) {
		if (_location->autoIndex())
			fileName += _location->getIndex();
		else {
			_errorCode = 404;
			throw (std::string) "AutoIndex off and no index file";
		}
	}
	if (stat(fileName.c_str(), &fileStat) == 0)
		_contentLen = fileStat.st_size;
	else {
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

void Response::initCgi() {
    _cgiExt = getExtension(_request.getPath());
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
    _envPtr.push_back(nullptr);
}

void Response::CGI() {
	int fd = 0;

	initCgi();
	_cgiFile = generateRandomName();
	_cgiFd = open(_cgiFile.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0644);
	if (_cgiFd < 0) {
		_errorCode = 500;
		throw (std::string) "failed to open a file";
	}
	write(_cgiFd, "HTTP/1.0 200 ok\r\n", 17);
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
		char *argv[] = {const_cast<char*>(_cgiExt.c_str()), const_cast<char*>(file.c_str()), nullptr};
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
	}
}

void Response::GET() {
	if (_errorCode != 200 && _errorCode != -1) {
		_isError = true;
		ERROR();
		return ;
	}
	try {
		getBody();
		_statusLine_Headers.append("HTTP/1.0 " + intToString(_errorCode) + " OK\r\n");
		_statusLine_Headers.append("Content-Type: " + _contentType + "\r\n");
		_statusLine_Headers.append("Content-Length: " + intToString(_contentLen) + "\r\n");
		_statusLine_Headers.append("Connection: Close\r\n");
		if (_contentLen > 0)
			_statusLine_Headers.append("\r\n");
	}
	catch (std::string error) {
		_isError = true;
		ERROR();
		std::cerr << error << std::endl;
	}
}

void Response::POST() {
	if (_errorCode != 200 && _errorCode != -1) {
		_isError = true;
		ERROR();
		return;
	}
	try
	{
		std::string uploadPath = _location->getUploadStore();
		if (uploadPath.empty()) {
			_errorCode = 403;
			throw (std::string) "File uploads are not configured for this location.";
		}

		struct stat dirStat;

		if (stat(uploadPath.c_str(), &dirStat) != 0) {
			if (mkdir(uploadPath.c_str(), 0755) != 0) {
				_errorCode = 500;
				throw (std::string) "Failed to create upload directory.";
			}
		} else if (!S_ISDIR(dirStat.st_mode)) {
			_errorCode = 500;
			throw (std::string) "Upload path exists but is not a directory.";
		}

		std::string requestPath = _request.getPath();
		std::string filename = requestPath.substr(requestPath.find_last_of('/') + 1);
		std::string filePath = uploadPath + "/" + filename;

		struct stat fileStat;
		if (stat(filePath.c_str(), &fileStat) == 0) {
			_errorCode = 409;
			throw (std::string) "Resource already exists at the target path.";
		}
		std::ofstream newFile(filePath.c_str(), std::ios::out | std::ios::binary);
		if (!newFile.is_open()) {
			_errorCode = 500;
			throw (std::string) "Failed to create the file on the server.";
		}
		std::ifstream& bodyFile = const_cast<std::ifstream&>(_request.getBodyFile());
		if (!bodyFile.is_open()) {
			_errorCode = 500;
			throw (std::string) "Failed to open request body file.";
		}
		newFile << bodyFile.rdbuf();
		bodyFile.close();
		newFile.close();
		if (!newFile.good()) {
			_errorCode = 500;
			unlink(filePath.c_str());
			throw (std::string) "An error occurred while writing to the file.";
		}
		_statusLine_Headers.clear();
		_statusLine_Headers.append("HTTP/1.0 204 No Content\r\n");
		_statusLine_Headers.append("Connection: Close\r\n\r\n");
		_contentLen = 0;
		
	}
	catch (const std::string& error) {
		_isError = true;
		ERROR();
		std::cerr << "POST Error: " << error << std::endl;
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
	_return.append("HTTP/1.0 " + intToString(_location->getReturn().first) + " Moved Permanently\n" + "Location: " + _location->getReturn().second);
	std::cout << _return << std::endl;
}

void Response::buildResponse() {
	
	if (isExtension(_request.getPath())) {
		_isCgi = true;
		CGI();
		return ;
	}
	if (_isRedirect) {
		REDIRECT();
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
	if (_isRedirect) {
		_responseState = DONE;
		return _return;
	}
	if (_isError) {
		if (_body.is_open())
			_body.close();
		_responseState = DONE;
		return _errorResponse;
	}
	else if (_isCgi && _responseState != DONE) {
			char buffer[BUFFER_SIZE];
			ssize_t bytes;

			bytes = read(_cgiFd, buffer, BUFFER_SIZE);
			if (bytes == 0) {
				close(_cgiFd);
				_responseState = DONE;
				return "";
			}	
			return std::string(buffer, bytes);
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
