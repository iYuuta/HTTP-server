#include "../../../includes/Response.hpp"

Response::~Response() {}

Response::Response(Request& req, std::map<int, std::string>& error, std::vector<Location>::iterator& location):
_request(req),
_location(location),
_errorPages(error),
_errorCode(-1),
_done(false),
_ErrorPageExists(true),
_readBody(false),
_responseState(STATUSLINE_HEADERS),
_bytesSend(0) {}

void Response::ERROR() {
	std::cout << _errorCode << std::endl;
	std::string errorFile = _errorPages[_errorCode];
	if (errorFile.length() == 0) {
		errorFile.append(DEF_ERROR);
		_ErrorPageExists = false;
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
		_statusLine_Headers.append("Content-Type: text/html\nContent-Length: 148\nConnection: close\r\n\r\n" + errorFile);
		_done = true;
		return ;
	}
	_statusLine_Headers.append("Content-Type: " + errorFile + "\r\n");
	_statusLine_Headers.append("Content-Length:" + intToString(_contentLen) + "\r\n");
}

void Response::createStatusLine() {
	_statusLine_Headers.append("HTTP/1.0 " + intToString(_errorCode) + "OK\r\n");
}

void Response::createHeaders() {
// 	_statusLine_Headers.append("Content-Type: text/html\r\nContent-Length: 147\r\nConnection: close\r\n\r\n""<!DOCTYPE html>\n"
// "<html>\n"
// "<head>\n"
// "    <title>Test Page</title>\n"
// "</head>\n"
// "<body>\n"
// "    <h1>Hello, this is a test page!</h1>\n"
// "    <p>Welcome to your webserver test.</p>\n"
// "</body>\n"
// "</html>\n");
	_statusLine_Headers.append("Content-Type: " + _contentType + "\r\n");
	_statusLine_Headers.append("Content-Length: " + intToString(_contentLen) + "\r\n");
	_statusLine_Headers.append("Connection: close\r\n\r\n");
}

void Response::getBody() {

	struct stat fileStat;
	std::string fileName = _location->getRoute() + _request.getPath();

	if (fileName[fileName.length() - 1] == '/') {
		if (_location->autoIndex())
			fileName += "index.html";
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
	// if (_request.getErrorCode()) {
	// 	ERROR();
	// 	_done = true;
	// 	return ;
	// }
	try {
		getBody();
		createStatusLine();
		createHeaders();
		_done = true;
	}
	catch (std::string) {
		ERROR();
	}
}

void Response::POST() {
	
}

void Response::DELETE() {

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
	if (_readBody) {
		char buffer[BUFFER_SIZE];
		size_t toRead = std::min(static_cast<size_t>(BUFFER_SIZE), _contentLen - _bytesSend);

		_body.read(buffer, toRead);
		std::streamsize bytesRead = _body.gcount();
		_bytesSend += bytesRead;
		if (_bytesSend >= _contentLen)
			_responseState = DONE;
		return std::string(buffer, bytesRead);
	}
	_responseState = BODY;
	_readBody = true;
	return _statusLine_Headers;
}

enums Response::getResponseState() const {
	return _responseState;
}

bool Response::isResponseDone() const {
	return _done;
}