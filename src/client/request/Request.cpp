#include "../../../includes/Request.hpp"

Request::Request():
_method(Unsupported),
_parseState(REQUESLINE),
_contentLen(0),
_receivedBytes(0),
_errorCode(-1),
_simpleRequest(false)
{
	_buffer.clear();
	_path.clear();
	_version.clear();
	_headers.clear();
	_queryString.clear();
	_bodyFileName.clear();
	_cookies.clear();
}

Request::~Request()
{
}

void Request::parseData(const char* data, size_t len)
{
	if (len == 0 && !_buffer.empty() && !_simpleRequest) {
		_errorCode = 400;
		throw (std::string) "Bad request";
	}
	if (_simpleRequest) {
		_parseState = DONE;
		return ;
	}
	_buffer.append(data, len);
	while (!_buffer.empty())
	{
		if (_parseState == REQUESLINE)
		{
			size_t pos = _buffer.find("\r\n");
			if (pos == std::string::npos)
				break ;
			addRequestLine( _buffer.substr(0, pos));
			if (_errorCode == 400)
				throw (std::string) "Bad request";
			if (_method == Unsupported) {
				_errorCode = 501;
				throw (std::string) "Unsupported method";
			}
			_buffer.erase(0, pos + 2);
			if (_simpleRequest)
				_parseState = DONE;
			else
				_parseState = HEADERS;
		}
		else if (_parseState == HEADERS)
		{
			size_t pos = _buffer.find("\r\n");

			if (pos == std::string::npos)
				break ;
			std::string header_line = _buffer.substr(0, pos);

			_buffer.erase(0, pos + 2);
			if (header_line.empty())
			{
				if (_headers["Content-Length"].empty()) {
					_errorCode = 400;
					throw (std::string) "Bad request";
				}
				if (_method == Post) {
					_bodyFileName = generateRandomName();
					_bodyOut.open(_bodyFileName.c_str(), std::ios::binary | std::ios::app);
					if (!_bodyOut.is_open()) {
						_errorCode = 500;
						throw (std::string) "Open failed";
					}
				}
				if (_contentLen > 0)
					_parseState = BODY;
				else
					_parseState = DONE;
			}
			else
				addHeaders(header_line);
			if (_errorCode == 400)
				throw (std::string) "Bad request";
		}
		else if (_parseState == BODY)
		{
			if (_contentLen == 0 || _receivedBytes >= _contentLen || _method != Post) {
				_parseState = DONE;
				break ;
			}
			size_t LeftOver = _contentLen - _receivedBytes;
			size_t ReadLen = std::min(LeftOver, _buffer.size());

			if (ReadLen == 0)
				break ;

			addBody(_buffer.substr(0, ReadLen), ReadLen);
			if (_errorCode == 500)
				throw (std::string) "Failed to open a file";
			_buffer.erase(0, ReadLen);
			_receivedBytes += ReadLen;
			if (_receivedBytes >= _contentLen)
			{
				_parseState = DONE;
				if (_bodyOut.is_open())
					_bodyOut.close();
			}
			break ;
		}
		if (_parseState == DONE)
			break ;
	}
}

bool Request::isValidRequestLine(const std::string& line) {
	size_t pos1 = line.find(' ');

	if (pos1 == std::string::npos)
		return false;
	size_t pos2 = line.find(' ', pos1 + 1);

	if (pos2 == std::string::npos) {
		std::string method = line.substr(0, pos1);
		std::string path = line.substr(pos1 + 1);

		if (method.empty() || path.empty())
			return false;
		return true;
	}
	if (line.find(' ', pos2 + 1) != std::string::npos)
		return false;
	std::string method = line.substr(0, pos1);
	std::string path = line.substr(pos1 + 1, pos2 - pos1 - 1);
	std::string version = line.substr(pos2 + 1);

	if (method.empty() || path.empty() || version.empty())
		return false;
	if (version != "HTTP/1.0" && version != "HTTP/1.1")
		return false;
	return true;
}

void Request::decodeUrl() {
	std::string newPath;
	
	for (size_t i = 0; i < _path.length(); i++) {
		if (_path[i] == '%') {
			if (i + 2 >= _path.length()) {
				_errorCode = 400;
				return ;
			}
			if (std::string("0123456789ABCDEFabcdef").find(_path[i + 1]) == std::string::npos ||
			std::string("0123456789ABCDEFabcdef").find(_path[i + 2]) == std::string::npos) {
				_errorCode = 400;
				return ;
			}
			newPath += hexToAscii(_path[i + 1], _path[i + 2]);
			i += 2;
		}
		else
			newPath += _path[i];
	}
	_path = newPath;
}

void Request::addRequestLine(const std::string &buff) {
	std::istringstream parser(buff);
	std::string method;
	size_t pos;

	if (!isValidRequestLine(buff)) {
		_errorCode = 400;
		return ;
	}
	parser >> method >> _path >> _version;
	if (_version.empty())
		_simpleRequest = true;
	if (method == "GET")
		_method = Get;
	else if (method == "POST")
		_method = Post;
	else if (method == "DELETE")
		_method = Delete;
	else
		_method = Unsupported;
	pos = _path.find('?');
	if (pos != std::string::npos) {
		_queryString = _path.substr(pos + 1);
		_path = _path.substr(0, pos);
	}
	if (_path.find('%') != std::string::npos)
		decodeUrl();
}

void Request::addHeaders(std::string buff)
{
	size_t pos = buff.find(":");
	if (pos == std::string::npos) {
		if (buff.empty() || (buff[0] != ' ' && buff[0] != '\t')) {
			_errorCode = 400;
			return ;
		}
		if (_headers.empty()) {
			_errorCode = 400;
			return ;
		}
		_headers.rbegin()->second += " " + trim(buff);
		return ;
	}
	std::string key = buff.substr(0, pos);
	if (!isKeyValid(key)) {
		_errorCode = 400;
		return ;
	}
	std::string value = trim(buff.substr(pos + 1));
	_headers[key] = value;
	if (key == "Content-Length")
	{
		if (value.empty()) {
			_errorCode = 400;
			return ;
		}
		char* endptr = NULL;
		if (!validcontentLength(value)) {
			_errorCode = 400;
			return ;
		}
		unsigned long long len = std::strtoull(value.c_str(), &endptr, 10);
		if (endptr == value.c_str() || *endptr != '\0') {
			_errorCode = 400;
			return ;
		}
		_contentLen = static_cast<size_t>(len);
	}
	else if (key == "Cookie")
	{
		if (value.empty()) {
			_errorCode = 400;
			return ;
		}
		parseCookie(value);
	}
}

void Request::addBody(const std::string& buff, size_t len) {
	_bodyOut.write(buff.data(), len);
}

void Request::checkForPathInfo(std::vector<Location>::iterator& location) {
	size_t index = 1;
	std::string ext;
	std::vector<std::string> exts = location->getExt();
	
	while (true) {
		size_t slash_pos = _path.find('/', index);

		if (slash_pos == std::string::npos)
			return ;
		size_t dot_pos = _path.find('.', index);

		if (dot_pos != std::string::npos && dot_pos < slash_pos)
			ext = _path.substr(dot_pos, slash_pos - dot_pos);
		index = slash_pos + 1;
		for (std::vector<std::string>::iterator it = exts.begin(); it != exts.end(); it++) {
			if (*it == ext) {
				_pathInfo = _path.substr(slash_pos + 1);
				_path = _path.substr(0, slash_pos);
				return ;
			}
		}
	}
}


void Request::setPath(const std::string& path) {
	_path = path;
}

const HttpRequestMethod& Request::getMeth() const
{
	return _method;
}

enums Request::getParseState() const
{
	return _parseState;
}

int Request::getErrorCode() const
{
	return _errorCode;
}

const std::string& Request::getPath() const
{
	return _path;
}

const std::string& Request::getPathInfo() const
{
	return _pathInfo;
}

const std::string& Request::getVersion() const
{
	return _version;
}

const std::string& Request::getFileName() {
	return _bodyFileName;
}

size_t Request::getContentLen() const
{
	return _contentLen;
}

size_t Request::getReceivedBytes() const
{
	return _receivedBytes;
}

const std::string Request::getHeader(const std::string& key)
{
	return _headers[key];
}

std::map<std::string, std::string>& Request::getHeaders()
{
	return _headers;
}

std::string& Request::getQueryStrings()
{
	return _queryString;
}

bool Request::isSimpleRequest() {
	return _simpleRequest;
}


void Request::parseCookie(const std::string &cookie)
{
	std::stringstream ss(cookie);
	std::string		  part;
	
	while(std::getline(ss, part, ';'))
	{
		size_t start = part.find_first_not_of(" \t");
		if (start != std::string::npos)
			part = part.substr(start);
		
		size_t equal = part.find('=');
		if (equal != std::string::npos)
		{
			std::string key = part.substr(0, equal);
			std::string value = part.substr(equal + 1);
			_cookies[key] = value;
		} 
	}
}


std::string Request::getCookie(const std::string &key)
{
	return (_cookies[key]);
}