#include "../../../includes/Response.hpp"

Response::~Response() {}

Response::Response(Request& req, std::map<int, std::string>& error, std::vector<Location>::iterator& location):
_request(req),
_location(location),
_errorPages(error),
_errorCode(200),
_done(false),
_ErrorPageExists(true),
_responseState(STATUSLINE_HEADERS),
_bytesSent(0) {}

void Response::ERROR() {
	std::cout << _errorCode << std::endl;
	std::string errorFile = _errorPages[_errorCode];
	struct stat fileStat;

	_statusLine_Headers.clear();
	if (_body.is_open())
		_body.close();
	if (errorFile.length() == 0) {
		errorFile.append(DEF_ERROR);
		_ErrorPageExists = false;
	}
	else {
		if (stat(errorFile.c_str(), &fileStat) == 0)
			_contentLen = fileStat.st_size;
		_body.open(errorFile.c_str(), std::ios::in | std::ios::binary);
		if (!_body.is_open()) {
			errorFile.append(DEF_ERROR);
			_ErrorPageExists = false;
			_errorCode = 500;
		}
	}
	switch (_errorCode) {
	case 400 :
		_statusLine_Headers.append("HTTP/1.0 400 Bad Request\r\n");
		break;
	case 404 :
		_statusLine_Headers.append("HTTP/1.0 404 Not Found\r\n");
		break;
	case 403 :
		_statusLine_Headers.append("HTTP/1.0 403 Forbidden\r\n");
		break;
	case 405 :
		_statusLine_Headers.append("HTTP/1.0 405 Method Not Allowed\r\n");
		break;
	case 413 :
		_statusLine_Headers.append("HTTP/1.0 413 Payload Too Large\r\n");
		break;
	case 500 :
		_statusLine_Headers.append("HTTP/1.0 500 Internal Server Error\r\n");
		break;
	case 501 :
		_statusLine_Headers.append("HTTP/1.0 501 Not Implemented\r\n");
		break;
	default:
		break;
	}

	if (!_ErrorPageExists) {
			_statusLine_Headers.append("Connection: close\r\n\r\n");
			_done = true;
			return;
		}
		_statusLine_Headers.append("Content-Type: text/html\r\n");
		_statusLine_Headers.append("Content-Length: " + intToString(_contentLen) + "\r\n");  // Fixed: added \r\n
		_statusLine_Headers.append("Connection: close\r\n\r\n");
		_done = true;
}

void Response::createStatusLine() {
	_statusLine_Headers.append("HTTP/1.0 " + intToString(_errorCode) + " OK\r\n");
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

	std::cout << fileName << std::endl;
	if (fileName[fileName.length() - 1] == '/') {
		if (_location->autoIndex())
			fileName += _location->getIndex();
		else {
			_errorCode = 404;
			throw (std::string) "AutoIndex off and no index file";
		}
		std::cout << fileName << std::endl;
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

void Response::GET() {
	if (_errorCode != 200 && _errorCode != -1) {
		ERROR();
		return ;
	}
	try {
		getBody();
		createStatusLine();
		createHeaders();
	}
	catch (std::string error) {
		ERROR();
		std::cerr << error << std::endl;
	}
}

void Response::POST() {
	
}

void Response::DELETE() {

	if (_errorCode != 200 && _errorCode != -1) {
        ERROR();
        return ;
    }
 	try {
        struct stat fileStat;
        std::string fileName = _location->getRoute() + _request.getPath();
        
        std::cout << "DELETE: " << fileName << std::endl;
        
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

		_statusLine_Headers.append("HTTP/1.0 204 No Content\r\n");
		_statusLine_Headers.append("Connection: close\r\n\r\n");
		_contentLen = 0;
		_done = true;
		// _responseState = DONE;

    }
    catch (std::string error) {
        ERROR();
        std::cerr << "DELETE error: " << error << std::endl;
    }
}

void Response::buildResponse() {
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
	if (_responseState == BODY && _bytesSent < _contentLen) {
		char buffer[BUFFER_SIZE];
		size_t toRead = std::min(static_cast<size_t>(BUFFER_SIZE), _contentLen - _bytesSent);

		_body.read(buffer, toRead);
		toRead = _body.gcount();
		_bytesSent += toRead;
		if (_bytesSent >= _contentLen)
		{
			// std::cout << "DONE\n";
			_responseState = DONE;
		}
		return std::string(buffer, toRead);
	}

	if (_responseState == BODY && _contentLen == 0) {
			_responseState = DONE;
			return "";
	}

	_responseState = BODY;
	return _statusLine_Headers;
}

void Response::setErrorCode(int error) {
	_errorCode = error;
}

enums Response::getResponseState() const {
	return _responseState;
}

bool Response::isResponseDone() const {
	return _done;
}