#include "../../../includes/Response.hpp"

#include <signal.h>

Response::~Response() {}

Response::Response(Request& req, std::map<int, std::string>& error, std::vector<Location>::iterator& location):
_request(req),
_location(location),
_errorPages(error),
_contentLen(-1),
_errorCode(200),
_cgiPid(0),
_cgiRunning(false),
_cgiExecuted(false),
_responseBuilt(false),
_index(false),
_isError(false),
_isCgi(false),
_isRedirect(false),
_errorPageExists(true),
_responseState(STATUSLINE_HEADERS),
_bytesSent(0) {
	_contentType.clear();
	_status.clear();
	_env.clear();
	_cgiExt.clear();
	_cgiFile.clear();
	_bodyLeftover.clear();
	_envPtr.clear();
	_errorResponse.clear();
	_return.clear();
	_headers.clear();
	_statusLine.clear();
	_boundary.clear();
	_multiparts.clear();
	_cookies.clear();
	_cookiesBuilt = false;

	_postState = POST_IDLE;
	_postIsMultipart = false;
	_multipartState = LOOKING_FOR_START_BOUNDARY;
	_multipartBuffer.clear();
	_multipartStartBoundary.clear();
	_multipartHeaderSeparator = "\r\n\r\n";

	
	_serverGeneratedName = false;
    _generatedUploadName.clear();

	_postCreatedNew = false;
    _multipartAnyCreated = false;
}

void Response::validateUploadPath(const std::string& uploadPath) {
	if (uploadPath.empty()) {
		_errorCode = 403;
		throw std::runtime_error("Uploads not configured for this location.");
	}

	struct stat dirStat;
	if (stat(uploadPath.c_str(), &dirStat) != 0)
			throw std::runtime_error("Upload directory does not exist.");

	else if (!S_ISDIR(dirStat.st_mode)) {
		_errorCode = 500;
		throw std::runtime_error("Upload path exists but is not a directory.");
	}
}

void Response::checkUploadFile(const std::string& path) {
	if (path.empty()) 
		return;
	if (access(path.c_str(), F_OK) != 0) {
		if (_uploadOutStream.is_open())
			_uploadOutStream.close();
		if (_postBodyStream.is_open())
			_postBodyStream.close();
		_errorCode = 409;
		throw std::runtime_error("Upload file was deleted during upload.");
	}
}

bool Response::stepRawUpload() {

	if (!_uploadOutStream.is_open())
	{
		_uploadOutStream.open(_postUploadPath.c_str(), std::ios::out | std::ios::binary);
		if (!_uploadOutStream) {
            _errorCode = 500;
            throw std::runtime_error("Failed to write uploaded data.");
        }
	}

	if (_uploadOutStream.is_open())
		checkUploadFile(_postUploadPath);
	char buffer[BUFFER_SIZE];
	_postBodyStream.read(buffer, sizeof(buffer));
	std::streamsize n = _postBodyStream.gcount();
	if (n > 0)
	{
		if (_uploadOutStream.is_open())
			checkUploadFile(_postUploadPath);
		_uploadOutStream.write(buffer, n);
		if (!_uploadOutStream) {
            _errorCode = 500;
            throw std::runtime_error("Failed to write uploaded data.");
        }
	}
	if (_postBodyStream.eof())
	{
		_uploadOutStream.close();
		_postBodyStream.close();
		return (true);
	}
	if (!_postBodyStream)
	{
		_errorCode = 500;
		throw std::runtime_error("Failed to read request body");
	}
	return (false);
}

static bool validBoundaryChar(char c)
{
	std::string forbidden = "()<>@,;:\\\"/[]?=";
	if (c < 32 || c > 126 || forbidden.find(c) != std::string::npos)
		return (false);
	return (true);
}

static bool validBoundary(const std::string &boundary)
{
	if (boundary.empty() || boundary.length() > 70 || boundary[boundary.length() - 1] == ' ') {
		return (false);
	}
	for (size_t i = 0; i < boundary.length(); i++)
	{
		if (!validBoundaryChar(boundary[i]))
			return (false);
	}
	return (true);
}

bool Response::extractBoundary(const std::string& contentTypeHeader, std::string& boundary) {
	size_t boundaryPos = contentTypeHeader.find("boundary=");
	if (boundaryPos == std::string::npos) {
		_errorCode = 400;
		return (false);
	}
	boundary = contentTypeHeader.substr(boundaryPos + 9);
	if (boundary.length() > 1 && boundary[0] == '"' && boundary[boundary.length() - 1] == '"')
		boundary = boundary.substr(1, boundary.length() - 2);
	if (!validBoundary(boundary)) {
		_errorCode = 400;
		return (false);
	}
	return (true);
}

void Response::parsePartHeaders(const std::string& headerStr, Multipart& part) {
	std::istringstream headerStream(headerStr);
	std::string line;

	while (std::getline(headerStream, line)) {
		if (!line.empty() && line[line.length() - 1] == '\r')
			line.erase(line.length() - 1);

		size_t colonPos = line.find(':');
		if (colonPos != std::string::npos) {
			std::string name = line.substr(0, colonPos);
			if (!isKeyValid(name)) {
				_errorCode = 400;
				throw std::runtime_error("Bad request");
			}
			std::string value = trim(line.substr(colonPos + 1));

			part.headers[name] = value;
			if (name == "q") {
				size_t namePos = value.find("name=\"");
				if (namePos != std::string::npos) {
					size_t nameEnd = value.find("\"", namePos + 6);
					if (nameEnd != std::string::npos)
						part.contentDispositionName = value.substr(namePos + 6, nameEnd - (namePos + 6));
				}
				size_t filenamePos = value.find("filename=\"");
				if (filenamePos != std::string::npos) {
					size_t filenameEnd = value.find("\"", filenamePos + 10);
					if (filenameEnd != std::string::npos) {
						part.contentDispositionFilename = value.substr(filenamePos + 10, filenameEnd - (filenamePos + 10));
						if (!part.contentDispositionFilename.empty())
							part.isFile = true;
					}
				}
			}
			else if (name == "Content-Type")
				part.contentType = value;
		}
		else {
			if (line.empty() || (line[0] != ' ' && line[0] != '\t')) {
				_errorCode = 400;
				throw std::runtime_error("Bad request");
			}
			if (part.headers.empty()) {
				_errorCode = 400;
				throw std::runtime_error("Bad request");
			}
			if (!part.headers.empty())
				part.headers.rbegin()->second = part.headers.rbegin()->second + " " + trim(line);

		}
	}
}

bool Response::stepMultipartUpload()
{
    if (_multipartBuffer.size() < BUFFER_SIZE * 2 && _postBodyStream)
    {
        char chunk[BUFFER_SIZE];
        _postBodyStream.read(chunk, BUFFER_SIZE);
        std::streamsize n = _postBodyStream.gcount();
        if (n > 0)
            _multipartBuffer.append(chunk, n);
    }

    if (_multipartState == LOOKING_FOR_START_BOUNDARY) {
        size_t pos = _multipartBuffer.find(_multipartStartBoundary);
        if (pos == std::string::npos)
		{
			if (!_postBodyStream && _multipartBuffer.find(_multipartStartBoundary) == std::string::npos)
			{
				_errorCode = 400;
				throw std::runtime_error("Bad Request");
			}
            return false;
		}
    
        _multipartBuffer.erase(0, pos + _multipartStartBoundary.length());
        if (_multipartBuffer.find("\r\n") == 0)
            _multipartBuffer.erase(0, 2);
        _multipartState = PARSING_HEADERS;
    }

    if (_multipartState == PARSING_HEADERS) {
        size_t pos = _multipartBuffer.find(_multipartHeaderSeparator);
        if (pos == std::string::npos) {
            if (!_postBodyStream) {
                _errorCode = 400;
                throw std::runtime_error("Malformed headers.");
            }
            return false;
        }
        std::string headers_str = _multipartBuffer.substr(0, pos);
        _multipartBuffer.erase(0, pos + _multipartHeaderSeparator.length());

        _multipartCurrentPart = Multipart();
        parsePartHeaders(headers_str, _multipartCurrentPart);

        if (_multipartCurrentPart.isFile && !_multipartCurrentPart.contentDispositionFilename.empty()) {
            std::string finalFilePath = _postUploadPath + "/" + _multipartCurrentPart.contentDispositionFilename;

			struct stat st;
			if (stat(finalFilePath.c_str(), &st) != 0) {
				_multipartAnyCreated = true;
			}
			_multipartCurrentPart.tempFilePath = finalFilePath;
            _uploadOutStream.open(finalFilePath.c_str(), std::ios::binary);
            if (!_uploadOutStream.is_open()) {
                _errorCode = 500;
                throw std::runtime_error("Failed to create file for upload.");
            }
        }
        _multipartState = STREAMING_BODY;
    }

	   if (_multipartState == STREAMING_BODY) {
        const std::string boundary_delimiter = std::string("\r\n") + _multipartStartBoundary;

		if (_uploadOutStream.is_open() && !_multipartCurrentPart.tempFilePath.empty())
			checkUploadFile(_multipartCurrentPart.tempFilePath);

        size_t pos = _multipartBuffer.find(boundary_delimiter);
        if (pos != std::string::npos) {
			if (_uploadOutStream.is_open()) {
				if (!_multipartCurrentPart.tempFilePath.empty())
					checkUploadFile(_multipartCurrentPart.tempFilePath);
                _uploadOutStream.write(_multipartBuffer.c_str(), pos);
                _uploadOutStream.close();
            }
            _multipartBuffer.erase(0, pos + boundary_delimiter.length());

            if (_multipartBuffer.find("--") == 0) {
                _multipartState = FINISHED;
            } else if (_multipartBuffer.find("\r\n") == 0) {
                _multipartBuffer.erase(0, 2);
                _multipartState = PARSING_HEADERS;
            }
        } else {
            size_t tail_size = _multipartStartBoundary.length() + 4;
            if (_multipartBuffer.length() > tail_size) {
                size_t write_size = _multipartBuffer.length() - tail_size;
				if (_uploadOutStream.is_open()) {
					if (!_multipartCurrentPart.tempFilePath.empty())
						checkUploadFile(_multipartCurrentPart.tempFilePath);
                    _uploadOutStream.write(_multipartBuffer.c_str(), write_size);
				}
                _multipartBuffer.erase(0, write_size);
            }
            if (!_postBodyStream && _multipartBuffer.find(boundary_delimiter) == std::string::npos) {
                _errorCode = 400;
                throw std::runtime_error("Unterminated part.");
            }
           return false;
        }
    }

    return (_multipartState == FINISHED);
}

void Response::postInit()
{

	if (_postState != POST_IDLE)
		return;

	std::string uploadPath = _location->getUploadStore();
    if (uploadPath.empty()){
        uploadPath = _location->getRoute();
	}
    
	validateUploadPath(uploadPath);
    _postUploadPath = uploadPath;


    std::string contentType = _request.getHeader("Content-Type");
    _postIsMultipart = (contentType.find("multipart/form-data") != std::string::npos);

	_postBodyStream.open(_request.getFileName().c_str(), std::ios::in | std::ios::binary);
    if (!_postBodyStream.is_open()) {
        _errorCode = 500;
        throw std::runtime_error("Failed to open request body file.");
    }

    if (_postIsMultipart) {
        if (!extractBoundary(contentType, _boundary)) {
            _errorCode = 400;
            throw std::runtime_error("Invalid or missing boundary.");
        }
        _multipartStartBoundary = std::string("--") + _boundary;
        _multipartState = LOOKING_FOR_START_BOUNDARY;
        _multipartBuffer.clear();
        _postState = POST_MULTIPART_STREAM;
        return;
    }

	std::string requestPath = _request.getPath();
    std::string filename;
    size_t slash = requestPath.find_last_of('/');
    if (slash != std::string::npos){
        filename = requestPath.substr(slash + 1);
	}

	_serverGeneratedName = false;
    _generatedUploadName.clear();

	    if (filename.empty()) {
			MIME _mime;
			std::string ext = _mime.getContentExt(contentType);
			_generatedUploadName = generateRandomName().substr(6) + ext;
			filename = _generatedUploadName;
			_serverGeneratedName = true;
    }

	_postUploadPath = _postUploadPath + "/" + filename;

	struct stat st;
	_postCreatedNew = (stat(_postUploadPath.c_str(), &st) != 0);
    _postState = POST_RAW_STREAM;

}


void Response::POST() {
	if (_errorCode != 200 && _errorCode != -1)
		return;
	try {
		if (_postState == POST_IDLE) {
			postInit();
		}
	
		bool done = false;
		if (_postState == POST_RAW_STREAM)
            done = stepRawUpload();
		else if (_postState == POST_MULTIPART_STREAM)
            done = stepMultipartUpload();

		if (!done) {
			return;
		}

		if (!_request.getFileName().empty()) {
			std::remove(_request.getFileName().c_str());
		}
		bool created = _postIsMultipart ? _multipartAnyCreated : (_serverGeneratedName || _postCreatedNew);
		std::string locationUrl;
		if (created) {
			if (_serverGeneratedName) {
				std::string pathPart = getFullPath(_location->getUrl(), _request.getPath());
				pathPart += _generatedUploadName;

				std::string host = _request.getHeader("Host");
				if (!host.empty()) {
					locationUrl = "http://" + host + pathPart;
					_headers.append("Location: " + locationUrl + "\r\n");
				} else {
					locationUrl = pathPart;
					_headers.append("Location: " + pathPart + "\r\n");
				}
			} else if (!_postIsMultipart) {
				std::string pathPart = getFullPath(_location->getUrl(), _request.getPath());
				std::string host = _request.getHeader("Host");
				if (!host.empty()) {
					locationUrl = "http://" + host + pathPart;
					_headers.append("Location: " + locationUrl + "\r\n");
				} else {
					locationUrl = pathPart;
					_headers.append("Location: " + pathPart + "\r\n");
				}
			}
			_statusLine.append("HTTP/1.0 201 Created\r\n");
		} else {
			_statusLine.append("HTTP/1.0 204 No Content\r\n");
		}

		if (created) {
			std::string body =
				"<!DOCTYPE html>"
				"<html><head><title>Upload Successful</title></head>"
				"<body>"
				"<h1>Uploaded successfully</h1>";
			if (!locationUrl.empty()) {
				body += "<p>Location: <a href=\"" + locationUrl + "\">" + locationUrl + "</a></p>";
			}
			body += "</body></html>";

			_headers.append("Content-Type: text/html\r\n");
			_headers.append("Content-Length: " + intToString(body.size()) + "\r\n");
			_bodyLeftover = body;
			_contentLen = body.size();
			_bytesSent += _bodyLeftover.size();
		} else {
			_headers.append("Content-Length: 0\r\n");
			_contentLen = 0;
		}

        for (size_t i = 0; i < _cookies.size(); i++)
            _headers.append("Set-Cookie: " + _cookies[i] + "\r\n");

        _headers.append("Connection: Close\r\n\r\n");

		_postState = POST_DONE;
		_responseBuilt = true;
	}
	catch (const std::exception &e)
	{
	    if (_postBodyStream.is_open())
        	_postBodyStream.close();
    	if (_uploadOutStream.is_open())
			_uploadOutStream.close();
		if (!_request.getFileName().empty())
			std::remove(_request.getFileName().c_str());

		ERROR();
		std::cerr << "Error: " << e.what() << std::endl;
	}
}


void Response::initCgi() {
	_cgiExt = getExtension(_request.getPath(), _location->getIndex());
	if (_cgiExt.empty()) {
		_errorCode = 501;
		throw (std::string) "unsupported cgi extention";
	}
	_env.clear();

	std::string fullPath = getFullPath(_location->getRoute(), _request.getPath());
	if (access(fullPath.c_str(), F_OK) == 0) {
		if (isDirectory(fullPath)) {
			if (_location->getIndex().empty()) {
				_errorCode = 404;
				throw (std::string) "file not found";
			}
			fullPath = getFullPath(_location->getRoute(), _location->getIndex());
			_index = true;
		}
    }
	else {
		_errorCode = 404;
		throw (std::string) "file not found";
	}
	_env.push_back("REQUEST_METHOD=" + methodToStr(_request.getMeth()));
	_env.push_back("CONTENT_LENGTH=" + intToString(_request.getContentLen()));
	_env.push_back("CONTENT_TYPE=" + _request.getHeader("Content-Type"));
	_env.push_back("SCRIPT_NAME=" + _location->getRoute() + _request.getPath());
	_env.push_back("SERVER_PROTOCOL=HTTP/1.0");
	_env.push_back("GATEWAY_INTERFACE=CGI/1.1");
	_env.push_back("SERVER_SOFTWARE=Webserv/1.0");
	_env.push_back("HTTP_HOST=" + _request.getHeader("Host"));
	_env.push_back("PATH_INFO=" + _request.getPathInfo());
	_env.push_back("HTTP_COOKIE=" + _request.getHeader("Cookie"));

	if (!_request.getQueryStrings().empty())
		_env.push_back("QUERY_STRING=" + _request.getQueryStrings());
	for (size_t i = 0; i < _env.size(); ++i)
		_envPtr.push_back(const_cast<char*>(_env[i].c_str()));
	_envPtr.push_back(NULL);
}

bool Response::addCgiHeaders(const std::string& line) {
	size_t pos = line.find(":");

	if (pos == std::string::npos) {
		if (line.empty() || (line[0] != ' ' && line[0] != '\t'))
			return false;
		if (_headers.empty())
			return false;
		_headers += " " + trim(line) + "\r\n";
		return true;
	}
	std::string key = line.substr(0, pos);

	if (!isKeyValid(key))
		return false;
	std::string value = trim(line.substr(pos + 1));
	if (strToLower(key) == "status" && _statusLine.empty()) {
		_statusLine.append("HTTP/1.0 " + value + "\r\n");
		return true;
	}
	if (strToLower(key) == "content-length") {
		if (value.empty())
			return true;
		char* endptr = NULL;
		unsigned long long len = std::strtoull(value.c_str(), &endptr, 10);
		if (endptr == value.c_str() || *endptr != '\0')
			return false;	
		_contentLen = static_cast<size_t>(len);
	}
	_headers += key + ": " + value + "\r\n";
	return true;
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
		std::remove(_request.getFileName().c_str());
		if (fd < 0) {
			std::remove(_cgiFile.c_str());
			close(_cgiFd);
			_errorCode = 500;
			throw (std::string) "failed to open a file";
		}
	}
	if (!_cgiRunning) {
		_cgiPid = fork();
		if (_cgiPid < 0) {
			if (fd != 0)
				close(fd);
			_errorCode = 500;
			std::remove(_cgiFile.c_str());
			close(_cgiFd);
			throw (std::string) "fork failed";
		}
		_cgiRunning = true;
		if (_cgiPid == 0) {
			if (chdir(_location->getRoute().c_str()) < 0) {
				if (fd != 0)
					close(fd);
				std::cerr << "failed to change cwd in the child process\n";
				std::remove(_cgiFile.c_str());
				close(_cgiFd);
				std::exit(500);
			}
			if (fd != 0) {
				dup2(fd, STDIN_FILENO);
				close(fd);
			}
			dup2(_cgiFd, STDOUT_FILENO);
			close(_cgiFd);
			std::string file;
			if (_index)
				file = _location->getIndex();
			else {
				file = _request.getPath();
				size_t pos = _request.getPath().find_first_not_of('/');
				if (pos != std::string::npos)
					file = file.erase(0, pos);
			}
			char *argv[] = {const_cast<char*>(_cgiExt.c_str()), const_cast<char*>(file.c_str()), NULL};
			execve(_cgiExt.c_str(), argv, _envPtr.data());
			std::exit (1);
		}
		else {
			if (fd != 0) {
				close(fd);
				fd = -1;
			}
		}
	}
}

bool Response::checkTimeOut() {
	struct stat st;

	if (stat(_cgiFile.c_str(), &st) == 0) {
		time_t mtime = st.st_mtime;
		time_t now = time(NULL);
		if (now - mtime > 15) {
			kill(_cgiPid, SIGTERM);
			close(_cgiFd);
			std::remove(_cgiFile.c_str());
			_errorCode = 504;
			return false;
		}
		return true;
	}
	close(_cgiFd);
	std::remove(_cgiFile.c_str());
	_errorCode = 500;
	return false;
}

void Response::monitorCgi() {
	int status = -1;

	pid_t ret = waitpid(_cgiPid, &status, WNOHANG);
	if (ret == 0)
		return ;

	_cgiRunning = false;
	_cgiExecuted = true;
	if (status != 0) {
		std::remove(_cgiFile.c_str());
		close(_cgiFd);
		if (!WIFEXITED(status) || WEXITSTATUS(status) != 0) {
			_errorCode = 500;
			return ;
		}
	}
	close(_cgiFd);
}

void Response::buildCgiResponse() {
	_body.open(_cgiFile.c_str(), std::ios::binary);
	size_t readBytes = 0;
	size_t bytesRead = 0;
	size_t pos = 0;
	size_t index = 0;
	struct stat st;
	int delimiter = 4;

	stat(_cgiFile.c_str(), &st);
	std::remove(_cgiFile.c_str());
	size_t fileSize = st.st_size;
	if (!_body.is_open()) {
		_errorCode = 500;
		std::cerr << "Error: Failed to open a file" << std::endl;
		ERROR();
		return ;
	}
	
	std::string buffer;
	while (!_body.eof()) {
		char chunk[512];
		_body.read(&chunk[0], 512);
		bytesRead = _body.gcount();
		readBytes += bytesRead;
		if (bytesRead <= 0)
			break;

		buffer.append(chunk);

		size_t pos = buffer.find("\r\n\r\n");
		if (pos == std::string::npos) {
			pos = buffer.find("\n\n");
			delimiter = 2;
		}
		if (pos != std::string::npos) {
			_bodyLeftover = buffer.substr(pos + delimiter);
			buffer.resize(pos + delimiter);
			break;
		}
	}
	while (true) {
		delimiter = 2;
		pos = buffer.find("\r\n");
		if (pos == std::string::npos) {
			pos = buffer.find("\n");
			delimiter = 1;
		}
		if (pos == std::string::npos) {
			_errorCode = 502;
			std::cerr << "invalid response from the child process" << std::endl;
			ERROR();
			return ;
		}

		std::string line = buffer.substr(index, pos - index);
		index = pos + delimiter;

		if (line == "\r\n" || line == "\n") {
			_headers.append("\r\n");
			break ;
		}

		if (!_statusLine.empty() && line.find("HTTP/1.") != std::string::npos) {
			_statusLine.append(line + "\r\n");
			continue ;
		}
		else if (!addCgiHeaders(line)) {
			_errorCode = 502;
			std::cerr << "invalid response from the child process" << std::endl;
			ERROR();
			return ;
		}
	}

	if (_statusLine.empty())
		_statusLine.append("HTTP/1.0 200 OK\r\n");
	if (_bodyLeftover.empty())
		_bodyLeftover = buffer.substr(index);
	if (_contentLen == -1)
		_contentLen = fileSize - readBytes + _bodyLeftover.size();
	_bytesSent += _bodyLeftover.size();
	_responseBuilt = true;
	return ;
}

void Response::CGI() {
	if (_cgiRunning) {
		monitorCgi();
		if (_errorCode == 500) {
			std::cerr << "child process terminated with a failure" << std::endl;
			ERROR();
			return ;
		}
		if (_cgiRunning && !checkTimeOut()) {
			std::cerr << "child process terminated due to a time out" << std::endl;
			ERROR();
			return ;
		}
		return ;
	}
	if (_cgiExecuted) {
		buildCgiResponse();
		return ;
	}
	try {
		initCgi();
		executeCgi();
	}
	catch (std::string err) {
		std::cerr << "Error: " << err << std::endl;
		ERROR();
		return ;
	}
}

void Response::ERROR() {
	std::string errorFile = _errorPages[_errorCode];
	struct stat fileStat;

	_isError = true;
	_responseBuilt = true;
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
		}
	}
	_errorResponse.append(_errors.getErrorMsg(_errorCode));
	if (!_errorPageExists) {
		std::string message = _errors.getErrorMsg(_errorCode).substr(9);
		message.resize(message.size() - 2);
		std::string body = std::string(ERROR_PAGE_START) + "<h1>" + message + "</h1>\n" + ERROR_PAGE_END;
		_errorResponse.append("Content-Type: text/html\r\n");
		_errorResponse.append("Content-Length: " + intToString(body.size()) + "\r\n");
		_errorResponse.append("Connection: Close\r\n\r\n");
		_errorResponse.append(body);
		return;
	}
	_errorResponse.append("Content-Type: text/html\r\n");
	_errorResponse.append("Content-Length: " + intToString(_contentLen) + "\r\n");

	for (size_t i = 0; i < _cookies.size(); i++)
		_errorResponse.append("Set-Cookie: " + _cookies[i] + "\r\n");

	_errorResponse.append("Connection: Close\r\n\r\n");
	std::ostringstream ss;
	ss << _body.rdbuf();
	_errorResponse += ss.str();
}

void Response::buildIndex() {
	DIR *dir = opendir((_location->getRoute() + _request.getPath()).c_str());
	if (!dir)
	{
		_errorCode = 403;
		throw (std::string) "Permission Denied";
		return ;
	}
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
	out << "<!DOCTYPE html>\n<html lang=\"en\" class=\"h-full\">\n<head>\n"
		<< "<meta charset=\"UTF-8\" />\n"
		<< "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\" />\n"
		<< "<meta name=\"color-scheme\" content=\"light dark\" />\n"
		<< "<title>Index of " << path << "</title>\n"
		<< "<style>\n"
		<< ":root{color-scheme:light dark;}\n"
		<< "html,body{height:100%;}body{margin:0;font-family:ui-sans-serif, system-ui, -apple-system, Segoe UI, Roboto, Helvetica, Arial, 'Apple Color Emoji', 'Segoe UI Emoji';background:linear-gradient(135deg,#f8fafc,#f1f5f9);color:#0f172a;-webkit-font-smoothing:antialiased;-moz-osx-font-smoothing:grayscale;}\n"
		<< "@media (prefers-color-scheme: dark){body{background:linear-gradient(135deg,#0f172a,#020617);color:#e2e8f0;}}\n"
		<< ".container{max-width:72rem;margin:0 auto;padding:1.5rem;}\n"
		<< ".card{border-radius:1rem;border:1px solid rgba(15,23,42,0.12);background:rgba(255,255,255,0.7);padding:1.5rem;box-shadow:0 10px 15px -3px rgba(0,0,0,.1),0 4px 6px -4px rgba(0,0,0,.1);}\n"
		<< "@media (prefers-color-scheme: dark){.card{background:rgba(2,6,23,.7);border-color:rgba(148,163,184,.2);}}\n"
		<< ".section-title{display:flex;align-items:center;gap:.75rem;margin-bottom:1rem;}\n"
		<< ".icon{display:inline-flex;height:2.5rem;width:2.5rem;border-radius:.5rem;align-items:center;justify-content:center;}\n"
		<< ".icon.blue{background:#dbeafe;color:#2563eb;}\n"
		<< "@media (prefers-color-scheme: dark){.icon.blue{background:rgba(37,99,235,.2);color:#93c5fd;}}\n"
		<< ".h2{font-size:1.125rem;font-weight:600;margin:0;}\n"
		<< ".table{width:100%;border-collapse:collapse;text-align:left;}\n"
		<< ".table thead th{padding:.75rem;border-bottom:1px solid rgba(15,23,42,0.2);font-weight:600;}\n"
		<< ".table tbody td{padding:.75rem;border-bottom:1px solid rgba(15,23,42,0.12);}\n"
		<< ".row{cursor:pointer;}\n"
		<< ".row:hover td{background:rgba(15,23,42,.03);}\n"
		<< "@media (prefers-color-scheme: dark){.table tbody td{border-color:#1f2937;}.row:hover td{background:rgba(255,255,255,.05);}}\n"
		<< ".h-5{height:1.25rem;}.w-5{width:1.25rem;}\n"
		<< "</style>\n"
		<< "</head>\n"
		<< "<body>\n"
		<< "<main class=\"container\">\n"
		<< "<div class=\"card\">\n"
		<< "<div class=\"section-title\">\n"
		<< "<span class=\"icon icon-sm blue\">\n"
		<< "<svg xmlns=\"http://www.w3.org/2000/svg\" class=\"h-5 w-5\" viewBox=\"0 0 24 24\" fill=\"currentColor\"><path d=\"M3 6a3 3 0 0 1 3-3h4l2 2h6a3 3 0 0 1 3 3v1H3V6Zm0 3h18v9a3 3 0 0 1-3 3H6a3 3 0 0 1-3-3V9Zm4 2v2h10v-2H7Z\" /></svg>\n"
		<< "</span>\n"
		<< "<h1 class=\"h2\">Index of " << path << "</h1>\n"
		<< "</div>\n"
		<< "<table class=\"table\">\n"
		<< "<thead><tr><th>Name</th></tr></thead>\n"
		<< "<tbody>\n";

	dirent *entry;
	while ((entry = readdir(dir)) != NULL) {
		std::string name = entry->d_name;

		if (name == "." || name == "..")
			continue;
		out << "<tr class=\"row\" onclick=\"window.location.href+='" << name << (isDirectory((_location->getRoute() + _request.getPath() + name).c_str()) ? "/" : "") << "'\">"
		<< "<td>" << name << "</td></tr>\n";
	}
	out << "</tbody>\n</table>\n</div>\n</main>\n</body>\n</html>\n";
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
	std::remove(fileName.c_str());
}

void Response::getBody() {

	struct stat fileStat;
	std::string fileName = getFullPath(_location->getRoute(), _request.getPath());

	if (isDirectory(fileName)) {
		if (!_location->getIndex().empty()) {
			fileName += _location->getIndex();
			if (isDirectory(fileName)) {
				_errorCode = 403;
				throw (std::string) "index is directory";
			}
			if (access(fileName.c_str(), F_OK) != 0 && _location->autoIndex()) {
				try {
					buildIndex();
					return ;
				}
				catch (std::string err) {
					throw err;
				}
			}
		}
		else if (_location->autoIndex()) {
			try {
				buildIndex();
				return ;
			}
			catch (std::string err) {
				throw err;
			}
		}
		else {
			_errorCode = 403;
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
		if (access(fileName.c_str(), F_OK) == 0) {
			_errorCode = 403;
			throw (std::string) "Forbidden";
		}
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
		_statusLine.append("HTTP/1.0 " + intToString(_errorCode) + " OK\r\n");
		_headers.append("Content-Type: " + _contentType + "\r\n");
		_headers.append("Content-Length: " + intToString(_contentLen) + "\r\n");

		for (size_t i = 0; i < _cookies.size(); i++)
			_headers.append("Set-Cookie: " + _cookies[i] + "\r\n");

		_headers.append("Connection: Close\r\n\r\n");
		_responseBuilt = true;
	}
	catch (std::string error) {
		std::cerr << "Error: " << error << std::endl;
		ERROR();
	}
}


void Response::DELETE() {

	if (_errorCode != 200 && _errorCode != -1) {
		ERROR();
		return ;
	}
 	try {
		struct stat fileStat;
		std::string fileName = getFullPath(_location->getRoute(), _request.getPath());

		if (stat(fileName.c_str(), &fileStat) != 0) {
			_errorCode = 404;
			throw (std::string) "file not found";
		}

		if (!S_ISREG(fileStat.st_mode)) {
			_errorCode = 403;
			throw (std::string) "not a regular file";
		}

		if (std::remove(fileName.c_str()) != 0) {
			_errorCode = 403;
			throw (std::string) "failed to delete file";
		}

		_statusLine.append("HTTP/1.0 204 No Content\r\n");

		for (size_t i = 0; i < _cookies.size(); i++)
		 _headers.append("Set-Cookie: " + _cookies[i] + "\r\n");

		_headers.append("Connection: Close\r\n\r\n");
		_contentLen = 0;
		_responseBuilt = true;
	}
	catch (std::string error) {
		ERROR();
		std::cerr << "Error: " << error << std::endl;
	}
}

void Response::REDIRECT() {
	int status = _location->getReturn().first;
	REDIRECTS _redirect;

	_return.append("HTTP/1.0 " + intToString(status) +
	" " + _redirect.getRedirectMsg(status) + "Location: " +
	_location->getReturn().second + "\r\n" + "Content-Length: 0\r\n\r\n");
}

void Response::simpleReqsponse() {
	if (_request.getMeth() == Get) {
		try {
			getBody();
			return ;
		}
		catch (std::string err) {
			std::cerr << "Error: " << err << std::endl;
			ERROR();
		}
	}
	else {
		_errorCode = 501;
		ERROR();
	}
}

void Response::buildCookies() 
{
	if (_cookiesBuilt)
		return;
	std::string sessionId = _request.getCookie("session-id");
	if (sessionId.empty())
	{
		std::stringstream ss;
		ss << "user-" << rand();
		std::string newSessionId = ss.str();
		
		std::string cookie = "session-id=" + newSessionId + "; Path=/";

		addCookie(cookie);
	}
	_cookiesBuilt = true;
}

void Response::buildResponse() {
	if (_responseBuilt) {
		return;
	}

	buildCookies();
	
	if (_errorCode != 200) {
		ERROR();
		return;
	}

	if (isExtension(_request.getPath(), _location->getIndex(), _location->getExt())) {
		if (_request.isSimpleRequest()) {
			_errorCode = 400;
			ERROR();
			return ;
		}
		CGI();
		return ;
	}
	else if (_isRedirect) {
		REDIRECT();
		_responseBuilt = true;
		return ;
	}
	else if (_request.isSimpleRequest()) {
		simpleReqsponse();
		_responseBuilt = true;
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
			ERROR();
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
	if (_request.isSimpleRequest()) {
		if (_contentLen == 0) {
			_responseState = DONE;
			return "";
		}
		if (_bytesSent < _contentLen) {
			char buffer[BUFFER_SIZE];
			size_t toRead = std::min(static_cast<ssize_t>(BUFFER_SIZE), _contentLen - _bytesSent);

			_body.read(buffer, toRead);
			size_t actuallyRead = _body.gcount();
			_bytesSent += actuallyRead;

			if (_bytesSent >= _contentLen) {
				_body.close();
				_responseState = DONE;
			}
			return std::string(buffer, actuallyRead);
		}
		if (_body.is_open())
			_body.close();
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
			return (_statusLine + _headers + _bodyLeftover);
		}
		case BODY: {
			if (_contentLen == 0) {
				_responseState = DONE;
				return "";
			}
			if (_bytesSent < _contentLen) {
				char buffer[BUFFER_SIZE];
				size_t toRead = std::min(static_cast<ssize_t>(BUFFER_SIZE), _contentLen - _bytesSent);

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

bool Response::isResponseBuilt() {
	return _responseBuilt;
}