#include "../../../includes/Request.hpp"

Request::Request() {};

Request::Request(int fd): _fd(fd), _method(NOTHING), _parse_state(REQUESLINE), _content_len(0), _received_bytes(0) {
	_buffer.clear();
	_path.clear();
	_version.clear();
	_body_file.clear();
}

Request::~Request() {}

void Request::ParseData(const char *data, size_t len) {
	_buffer.append(data, len);
	while (!_buffer.empty()) {
		if (_parse_state == REQUESLINE) {
			size_t pos = _buffer.find("\r\n");

			if (pos != std::string::npos) {
				std::string request_line = _buffer.substr(0, pos);
				_buffer.erase(0, pos + 2);
				AddRequestLine(request_line);
				_parse_state = HEADERS;
			}
			else
				break ;
		}
		else if (_parse_state == HEADERS) {
			size_t pos = _buffer.find("\r\n");

			if (pos == std::string::npos)
				break ;
			std::string header_line = _buffer.substr(0, pos);

			_buffer.erase(0, pos + 2);
			if (header_line.empty()) {
				if (_content_len > 0)
					_parse_state = BODY;
				else
					_parse_state = DONE;
			}
			else
				AddHeaders(header_line);
		}
		else if (_parse_state == BODY) {
			size_t LeftOver = _content_len - _received_bytes;
			size_t ReadLen = std::min(LeftOver, _buffer.size());

			AddBody(_buffer.substr(0, ReadLen), ReadLen);
			_buffer.erase(0, ReadLen);
			_received_bytes += ReadLen;
			if (_received_bytes >= _content_len) {
				_parse_state = DONE;
				_body.close();
			}
			break ;
		}
		if (_parse_state == DONE) {
			std::cout << _headers["Content-Length"] << std::endl << _content_len << std::endl;
			break ;
		}
	}
}

void Request::AddRequestLine(std::string buff) {
	std::istringstream	parser(buff);
	std::string			method;

	if (!(parser >> method >> _path >> _version))
		throw InvalidRequestLine();
	if (method == "GET")
		_method = GET;
	else if (method == "POST")
		_method = POST;
	else if (method == "DELETE")
		_method = DELETE;
	else
		_method = UNSUPPORTED;
}

static std::string trim(const std::string& s) {
	size_t start = 0;

	while (start < s.size() && (s[start] == ' ' || s[start] == '\t'))
		++start;
	if (start == s.size())
		return "";

	size_t end = s.size() - 1;

	while (end > start && (s[end] == ' ' || s[end] == '\t'))
		--end;
	return s.substr(start, end - start + 1);
}

void Request::AddHeaders(std::string buff) {
	size_t pos = buff.find(":");
	if (pos == std::string::npos)
		throw InvalidHeader();
	std::string key = trim(buff.substr(0, pos));
	std::string value = trim(buff.substr(pos + 1));
	_headers[key] = value;
	if (key == "Content-Length") {
		if (value.empty())
			throw InvalidHeader();
		char* endptr = NULL;
		unsigned long long parsed = std::strtoull(value.c_str(), &endptr, 10);
		if (endptr == value.c_str() || *endptr != '\0')
			throw InvalidHeader();
		if (parsed > static_cast<unsigned long long>(SIZE_MAX))
			throw InvalidHeader();
		_content_len = static_cast<size_t>(parsed);
	}
}

void Request::AddBody(const std::string& buff, size_t len) {
	if (!_body.is_open()) {
    _body_file = "/tmp/body_" + std::to_string(_fd) + ".tmp";
    _body.open(_body_file.c_str(), std::ios::binary | std::ios::out);
    if (!_body)
      throw OpenFailed();
	}
	_body.write(buff.data(), len);
	if (!_body)
		throw WriteError();
}

enums Request::GetMeth() const {
	return _method;
}

enums Request::GetParseState() const {
	return _parse_state;
}

const std::string& Request::GetPath() const {
	return _path;
}

const std::string& Request::GetVersion() const {
	return _version;
}

size_t Request::GetContentLen() const {
	return _content_len;
}

size_t Request::GetReceivedBytes() const {
	return _received_bytes;
}

const std::string Request::GetHeader(const std::string& key) {
	return _headers[key];
}

const std::ifstream& Request::GetBodyFile() {
	_body_in.open(_body_file, std::ios::binary);
	if (!_body_in.is_open())
		throw OpenFailed();
	return _body_in;
}

const char *Request::InvalidRequestLine::what() const throw() {
	return "Invalid request line";
}

const char *Request::InvalidHeader::what() const throw() {
	return "Invalid header";
}

const char *Request::OpenFailed::what() const throw() {
	return "open failed";
}

const char *Request::WriteError::what() const throw() {
	return "write failed";
}

int	Request::GetFd() const {
	return _fd;
}

std::map<std::string, std::string> Request::GetHeaders() const {
	return _headers;
}
