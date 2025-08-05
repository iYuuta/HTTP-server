#include "../../../includes/Request.hpp"

Request::Request():
	 _method(Unsupported),_parseState(REQUESLINE), _contentLen(0), _receivedBytes(0), _errorCode(-1), _simpleRequest(false)
{
	_buffer.clear();
	_path.clear();
	_version.clear();
	_bodyFileName.clear();
	_queryString.clear();
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
				_errorCode = 405;
				throw (std::string) "Unsupported method";
			}
			_buffer.erase(0, pos + 2);
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
				if (_contentLen > 0)
					_parseState = BODY;
				else
					_parseState = DONE;
			}
			else
				addHeaders(header_line);
		}
		else if (_parseState == BODY)
		{
			if (_contentLen == 0 || _receivedBytes >= _contentLen) {
				_parseState = DONE;
				break ;
			}
			size_t LeftOver = _contentLen - _receivedBytes;
			size_t ReadLen = std::min(LeftOver, _buffer.size());

			if (ReadLen == 0)
				break ;

			addBody(_buffer.substr(0, ReadLen), ReadLen);
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

void Request::addRequestLine(const std::string &buff) {
	std::istringstream parser(buff);
	std::string method;

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
	size_t pos = _path.find('?');
	if (pos != std::string::npos) {
		_queryString = _path.substr(pos + 1);
		_path = _path.substr(0, pos);
	}
}

void Request::addHeaders(std::string buff)
{
	size_t pos = buff.find(":");
	if (pos == std::string::npos) {
		if (buff.empty() || (buff[0] != ' ' && buff[0] != '\t')) {
			_errorCode = 400;
			throw std::string("Bad request");
		}
		if (_headers.empty()) {
			_errorCode = 400;
			throw std::string("Bad request");
		}
		_headers.rbegin()->second += " " + trim(buff);
		return ;
	}
	std::string key = buff.substr(0, pos);
	if (!isKeyValid(key)) {
		_errorCode = 400;
		throw std::string("Bad request");
	}
	std::string value = trim(buff.substr(pos + 1));
	_headers[key] = value;
	if (key == "Content-Length")
	{
		if (value.empty()) {
			_errorCode = 400;
			throw (std::string) "Bad request";
		}
		char* endptr = NULL;
		unsigned long long len = std::strtoull(value.c_str(), &endptr, 10);
		if (endptr == value.c_str() || *endptr != '\0') {
			_errorCode = 400;
			throw (std::string) "Bad request";
		}
		_contentLen = static_cast<size_t>(len);
	}
}

void Request::addBody(const std::string& buff, size_t len)
{
	if (!_bodyOut.is_open())
	{
		_bodyFileName = generateRandomName();
		_bodyOut.open(_bodyFileName.c_str(), std::ios::binary | std::ios::app);
		if (!_bodyOut) {
			_errorCode = 500;
			throw (std::string) "failed to open a file";
		}
	}
	_bodyOut.write(buff.data(), len);
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

const std::ifstream& Request::getBodyFile()
{
	_bodyIn.open(_bodyFileName.c_str(), std::ios::binary);
	if (!_bodyIn.is_open()) {
		_errorCode = 404;
		throw (std::string) "failed to open a file";
	}
	return _bodyIn;
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