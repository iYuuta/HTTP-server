#include "../../../includes/Response.hpp"

Response::~Response() {}

Response::Response(Request& req, std::map<int, std::string>& error, std::vector<Location>::iterator& location):
_request(req),
_location(location),
_errorPages(error),
_errorCode(200),
_isError(false),
_isCgi(false),
_errorPageExists(true),
_responseState(STATUSLINE_HEADERS),
_bytesSent(0) {
	_errorResponse.clear();
	_statusLine_Headers.clear();
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
	case 500 :
		_errorResponse.append("HTTP/1.0 500 Internal Server Error\r\n");
		break;
	case 501 :
		_errorResponse.append("HTTP/1.0 501 Not Implemented\r\n");
		break;
	default:
		break;
	}
	if (!_errorPageExists) {
		_errorResponse.append("Content-Type: text/html\r\nContent-Length: 147\r\nConnection: close\r\n\r\n");
		_errorResponse.append(DEF_ERROR);
		return;
	}
	_errorResponse.append("Content-Type: text/html\r\n");
	_errorResponse.append("Content-Length: " + intToString(_contentLen) + "\r\n");
	_errorResponse.append("Connection: close\r\n\r\n");
	std::ostringstream ss;
	ss << _body.rdbuf();
	_errorResponse += ss.str();
}

void Response::createHeaders() {
	_statusLine_Headers.append("Content-Type: " + _contentType + "\r\n");
	_statusLine_Headers.append("Content-Length: " + intToString(_contentLen) + "\r\n");
	_statusLine_Headers.append("Connection: close\r\n");
	if (_contentLen > 0)
		_statusLine_Headers.append("\r\n");
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

void Response::CGI() {
	int cgiPipe[2];

	std::cout << "cgi running\n";
	if (pipe(cgiPipe) < 0) {
		_errorCode = 500;
		throw (std::string) "pipe failed";
	}
	int pid = fork();
	if (pid < 0) {
		_errorCode = 500;
		throw (std::string) "fork failed";
	}
	else if (pid == 0) {
		if (chdir((_location->getRoute()).c_str()) < 0) 
			exit (500);
		dup2(cgiPipe[1], STDOUT_FILENO);
		close(cgiPipe[1]);
		close(cgiPipe[0]);
		char* argv[] = {(char *)_request.getPath().c_str(), NULL};
		execve(_request.getPath().substr(1).c_str(), argv, NULL);
		exit(500);
	}
	else {
		int status;
		int value;
		close(cgiPipe[1]);
		waitpid(pid, &status, 0);
		if (WIFEXITED(status))
			value = WEXITSTATUS(status);
		if (WIFSIGNALED(status))
			value = WTERMSIG(status);
		if (value != 0) {
			close(cgiPipe[0]);
			_errorCode = 500;
			throw (std::string) "cgi failed";
		}
		std::string name = generateRandomName();
		_cgiResponse.open(name.c_str(), std::ios::binary | std::ios::out);
		if (!_cgiResponse.is_open()) {
			close(cgiPipe[0]);
			_errorCode = 500;
			throw (std::string) "failed to open a file";
		}
		_cgiResponse << "Status: 200 OK\r\n";
		_contentLen += 16;
		char buffer[BUFFER_SIZE];
    	ssize_t bytesRead;

	    while ((bytesRead = read(cgiPipe[0], buffer, BUFFER_SIZE)) > 0) {
			_contentLen += bytesRead;
    	    _cgiResponse.write(buffer, bytesRead);
	    }
	    if (bytesRead < 0) {
    	    _errorCode = 500;
			throw (std::string) "Read error";
    	}
		_cgiResponse.flush();
		_contentLen = 199 + 16;
		_cgiResponse.close();
		_cgiResponse.open(name.c_str(), std::ios::binary | std::ios::out);
		if (!_cgiResponse.is_open()) {
			close(cgiPipe[0]);
			_errorCode = 500;
			throw (std::string) "failed to open a file";
		}
		std::ostringstream ss;
    	ss << _cgiResponse.rdbuf();
		std::cout << "-> " << ss.str() << std::endl;
	}
}

void Response::GET() {
	if (_errorCode != 200 && _errorCode != -1) {
		_isError = true;
		ERROR();
		return ;
	}
	try {
		if (_isCgi) {
			CGI();
			return ;
		}
		getBody();
		_statusLine_Headers.append("HTTP/1.0 " + intToString(_errorCode) + " OK\r\n");
		createHeaders();
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
		_statusLine_Headers.append("Connection: close\r\n\r\n");
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
		_statusLine_Headers.append("Connection: close\r\n\r\n");
		_contentLen = 0;

	}
	catch (std::string error) {
		_isError = true;
		ERROR();
		std::cerr << error << std::endl;
	}
}

void Response::buildResponse() {
	
	if (_location->getUrl() == "/cgi-bin")
		_isCgi = true;
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
			break;
	}
}

std::string Response::getResponse() {
	if (_isError) {
		if (_body.is_open())
			_body.close();
		_responseState = DONE;
		return _errorResponse;
	}
	else if (_isCgi && _responseState != DONE) {
		if (_contentLen == 0) {
			_responseState = DONE;
			return "";
		}
		if (_bytesSent < _contentLen) {
			char buffer[BUFFER_SIZE];
			size_t toRead = std::min(static_cast<size_t>(BUFFER_SIZE), _contentLen - _bytesSent);

			_cgiResponse.read(buffer, toRead);
			size_t actuallyRead = _cgiResponse.gcount();
			if (_cgiResponse.eof())
				actuallyRead = toRead;
			_bytesSent += actuallyRead;

			if (_bytesSent >= _contentLen) {
				_cgiResponse.close();
				_responseState = DONE;
			}				
			return std::string(buffer, actuallyRead);
		}
		_responseState = DONE;
		return "";
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

enums Response::getResponseState() const {
	return _responseState;
}
