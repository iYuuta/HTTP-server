#include "../../../includes/Request.hpp"

Request::Request()
{
};

Request::Request(size_t maxBodySize):
	_parseState(REQUESLINE), _contentLen(0), _receivedBytes(0)
{
	_buffer.clear();
	_path.clear();
	_version.clear();
	_bodyFileName.clear();
}

Request::~Request()
{
}

void Request::parseData(const char* data, size_t len)
{
	_buffer.append(data, len);
	while (!_buffer.empty())
	{
		if (_parseState == REQUESLINE)
		{
			size_t pos = _buffer.find("\r\n");

			if (pos != std::string::npos)
			{
				std::string request_line = _buffer.substr(0, pos);
				_buffer.erase(0, pos + 2);
				addRequestLine(request_line);
				_parseState = HEADERS;
			}
			else
				break ;
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
			size_t LeftOver = _contentLen - _receivedBytes;
			size_t ReadLen = std::min(LeftOver, _buffer.size());

			addBody(_buffer.substr(0, ReadLen), ReadLen);
			_buffer.erase(0, ReadLen);
			_receivedBytes += ReadLen;
			if (_receivedBytes >= _contentLen)
			{
				_parseState = DONE;
				_bodyOut.close();
			}
			break ;
		}
		if (_parseState == DONE)
		{
			break ;
		}
	}
}

void Request::addRequestLine(std::string buff)
{
	std::istringstream parser(buff);
	std::string method;

	if (!(parser >> method >> _path >> _version))
		throw InvalidRequestLine();
	if (method == "GET")
		_method = Get;
	else if (method == "POST")
		_method = Post;
	else if (method == "DELETE")
		_method = Delete;
	else
		_method = Unsupported;
}

void Request::addHeaders(std::string buff)
{
	size_t pos = buff.find(":");
	if (pos == std::string::npos)
		throw InvalidHeader();
	std::string key = trim(buff.substr(0, pos));
	std::string value = trim(buff.substr(pos + 1));
	_headers[key] = value;
	if (key == "Content-Length")
	{
		if (value.empty())
			throw InvalidHeader();
		char* endptr = NULL;
		unsigned long long len = std::strtoull(value.c_str(), &endptr, 10);
		if (endptr == value.c_str() || *endptr != '\0')
			throw InvalidHeader();
		_contentLen = static_cast<size_t>(len);
	}
}

void Request::addBody(const std::string& buff, size_t len)
{
	if (!_bodyOut.is_open())
	{
		_bodyFileName = "/tmp/." + generateRandomName();
		_bodyOut.open(_bodyFileName.c_str(), std::ios::binary | std::ios::out);
		if (!_bodyOut)
			throw OpenFailed();
	}
	_bodyOut.write(buff.data(), len);
	if (!_bodyOut)
		throw WriteError();
}

const HttpRequestMethod& Request::getMeth() const
{
	return _method;
}

enums Request::getParseState() const
{
	return _parseState;
}

const std::string& Request::getPath() const
{
	return _path;
}

const std::string& Request::getVersion() const
{
	return _version;
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
	if (!_bodyIn.is_open())
		throw OpenFailed();
	return _bodyIn;
}

const char* Request::InvalidRequestLine::what() const throw()
{
	return "Invalid request line";
}

const char* Request::InvalidHeader::what() const throw()
{
	return "Invalid header";
}

const char* Request::OpenFailed::what() const throw()
{
	return "open failed";
}

const char* Request::WriteError::what() const throw()
{
	return "write failed";
}

std::map<std::string, std::string> Request::getHeaders() const
{
	return _headers;
}
