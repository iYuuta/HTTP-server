# include "../../includes/utils.hpp"

MIME::MIME() {
	_mime[".aac"] = "audio/aac";
	_mime[".abw"] = "application/x-abiword";
	_mime[".arc"] = "application/x-freearc";
	_mime[".avi"] = "video/x-msvideo";
	_mime[".azw"] = "application/vnd.amazon.ebook";
	_mime[".bin"] = "application/octet-stream";
	_mime[".bmp"] = "image/bmp";
	_mime[".bz"] = "application/x-bzip";
	_mime[".bz2"] = "application/x-bzip2";
	_mime[".csh"] = "application/x-csh";
	_mime[".css"] = "text/css";
	_mime[".csv"] = "text/csv";
	_mime[".doc"] = "application/msword";
	_mime[".docx"] = "application/vnd.openxmlformats-officedocument.wordprocessingml.document";
	_mime[".eot"] = "application/vnd.ms-fontobject";
	_mime[".epub"] = "application/epub+zip";
	_mime[".gz"] = "application/gzip";
	_mime[".gif"] = "image/gif";
	_mime[".htm"] = "text/html";
	_mime[".html"] = "text/html";
	_mime[".ico"] = "image/vnd.microsoft.icon";
	_mime[".ics"] = "text/calendar";
	_mime[".jar"] = "application/java-archive";
	_mime[".jpeg"] = "image/jpeg";
	_mime[".jpg"] = "image/jpeg";
	_mime[".js"] = "text/javascript";
	_mime[".json"] = "application/json";
	_mime[".jsonld"] = "application/ld+json";
	_mime[".mid"] = "audio/midi";
	_mime[".midi"] = "audio/midi";
	_mime[".mjs"] = "text/javascript";
	_mime[".mp3"] = "audio/mpeg";
	_mime[".mpeg"] = "video/mpeg";
	_mime[".mpkg"] = "application/vnd.apple.installer+xml";
	_mime[".odp"] = "application/vnd.oasis.opendocument.presentation";
	_mime[".ods"] = "application/vnd.oasis.opendocument.spreadsheet";
	_mime[".odt"] = "application/vnd.oasis.opendocument.text";
	_mime[".oga"] = "audio/ogg";
	_mime[".ogv"] = "video/ogg";
	_mime[".ogx"] = "application/ogg";
	_mime[".opus"] = "audio/ogg";
	_mime[".otf"] = "font/otf";
	_mime[".png"] = "image/png";
	_mime[".pdf"] = "application/pdf";
	_mime[".php"] = "application/x-httpd-php";
	_mime[".ppt"] = "application/vnd.ms-powerpoint";
	_mime[".pptx"] = "application/vnd.openxmlformats-officedocument.presentationml.presentation";
	_mime[".rar"] = "application/vnd.rar";
	_mime[".rtf"] = "application/rtf";
	_mime[".sh"] = "application/x-sh";
	_mime[".svg"] = "image/svg+xml";
	_mime[".swf"] = "application/x-shockwave-flash";
	_mime[".tar"] = "application/x-tar";
	_mime[".tif"] = "image/tiff";
	_mime[".tiff"] = "image/tiff";
	_mime[".ts"] = "video/mp2t";
	_mime[".ttf"] = "font/ttf";
	_mime[".txt"] = "text/plain";
	_mime[".vsd"] = "application/vnd.visio";
	_mime[".wav"] = "audio/wav";
	_mime[".weba"] = "audio/webm";
	_mime[".webm"] = "video/webm";
	_mime[".webp"] = "image/webp";
	_mime[".woff"] = "font/woff";
	_mime[".woff2"] = "font/woff2";
	_mime[".xhtml"] = "application/xhtml+xml";
	_mime[".xls"] = "application/vnd.ms-excel";
	_mime[".xlsx"] = "application/vnd.openxmlformats-officedocument.spreadsheetml.sheet";
	_mime[".xml"] = "text/xml";
	_mime[".mp4"] = "video/mp4";
	_mime[".zip"] = "application/zip";
	_mime[".7z"] = "application/x-7z-compressed";
}

Exec::Exec() {
	_exec[".sh"]	= "/bin/sh";
	_exec[".bash"]  = "/bin/bash";
	_exec[".zsh"]   = "/bin/zsh";
	_exec[".py"]	= "/usr/bin/python3";
	_exec[".php"]   = "/usr/bin/php";
	_exec[".pl"]	= "/usr/bin/perl";
	_exec[".js"]	= "/usr/local/bin/node";
	_exec[".awk"]   = "/usr/bin/awk";
}

ERRORS::ERRORS() {
	_errors[400] = "HTTP/1.0 400 Bad Request\r\n";
	_errors[404] = "HTTP/1.0 404 Not Found\r\n";
	_errors[403] = "HTTP/1.0 403 Forbidden\r\n";
	_errors[405] = "HTTP/1.0 405 Method Not Allowed\r\n";
	_errors[413] = "HTTP/1.0 413 Payload Too Large\r\n";
	_errors[501] = "HTTP/1.0 501 Not Implemented\r\n";
	_errors[409] = "HTTP/1.0 409 Conflict\r\n";
	_errors[502] = "HTTP/1.0 502 Bad Gateway\r\n";
	_errors[504] = "HTTP/1.0 504 Gateway Timeout\r\n";
	_errors[500] = "HTTP/1.0 500 Internal Server Error\r\n";
}

REDIRECTS::REDIRECTS() {
	_redirect[300] = "Multiple Choices\r\n";
	_redirect[301] = "Moved Permanently\r\n";
	_redirect[302] = "Found\r\n";
	_redirect[303] = "See Other\r\n";
	_redirect[304] = "Not Modified\r\n";
	_redirect[305] = "Use Proxy\r\n";
	_redirect[306] = "Switch Proxy\r\n";
	_redirect[307] = "Temporary Redirect\r\n";
	_redirect[308] = "Permanent Redirect\r\n";
}

std::vector<std::string> split(const std::string& s, const char delimiter)
{
	std::vector<std::string> tokens;
	size_t start = 0;
	size_t end;

	while ((end = s.find(delimiter, start)) != std::string::npos)
	{
		tokens.push_back(s.substr(start, end - start));
		start = end + 1;
	}

	tokens.push_back(s.substr(start));
	return tokens;
}

std::vector<std::string> splitNumber(const std::string& s)
{
	std::vector<std::string> tokens;
	if (!std::isdigit(s[0]))
		return (tokens);
	for (size_t i = 0; i < s.size(); ++i)
	{
		if (!std::isdigit(s[i]))
		{
			tokens.push_back(s.substr(0, i));
			tokens.push_back(s.substr(i));
			break;
		}
	}
	return tokens;
}

std::string intToString(int n) {
	std::ostringstream oss;
	oss << n;
	return oss.str();
}

long long atoill(const std::string& s)
{
	errno = 0;
	char* ptr;
	const long long value = std::strtoll(s.c_str(), &ptr, 10);
	if (errno != 0 || *ptr != '\0')
		throw std::invalid_argument(s);
	return (value);
}

std::string getContentType(const std::string& fileName) {
	size_t pos = fileName.find_last_of('.');

	if (pos == std::string::npos)
		return "application/octet-stream";

	MIME _mime;
	std::string ext = fileName.substr(pos);

	return (_mime.getMIME(ext));
}

std::string trim(const std::string& s) {
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

std::string generateRandomName() {
	const std::string characters = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
	static bool seeded = false;

	if (!seeded) {
		std::srand(std::time(NULL));
		seeded = true;
	}
	while (true) {
		std::string result = "/tmp/";
		for (size_t i = 0; i < 16; ++i)
			result += characters[std::rand() % characters.size()];

		if (access(result.c_str(), F_OK) != 0)
			return result;
	}
}

bool locationExists(const std::string &path) {
	return (access(path.c_str(), F_OK) == 0);
}

void removeSegment(std::string& buff) {
	size_t pos;

	if (buff.empty())
		return ;
	pos = buff.find_last_of('/');
	if (pos == std::string::npos) {
		buff = "";
		return ;
	}
	buff.resize(pos);
}

void normalizePath(std::string& path) {
	std::string newPath, buff, tmpPath;
	size_t slashPos = 0, pos;
	bool lastSlash = false;

	for (size_t i = 0; i < path.length(); i++) {
		if (path[i] == '/' && !lastSlash) {
			lastSlash = true;
			tmpPath += path[i];
		}
		else if (path[i] != '/') {
			lastSlash = false;
			tmpPath += path[i];
		}
	}

	if (tmpPath.size() > 1 && tmpPath[tmpPath.length() - 1] == '/')
		tmpPath.resize(tmpPath.size() - 1);

    if (tmpPath.empty())
		tmpPath = "/";

	while (((pos = tmpPath.find("../", 0)) == 1) || ((pos = tmpPath.find("./", 0)) == 1)) {
		if ((pos = tmpPath.find("../", 0)) == 1)
			tmpPath = "/" + tmpPath.substr(4);
		else if ((pos = tmpPath.find("./", 0)) == 1)
			tmpPath = "/" + tmpPath.substr(3);
	}

	while (true) {
		slashPos = tmpPath.find('/');
		if (slashPos == std::string::npos)
			break ;
		pos = tmpPath.find('/', slashPos + 1);
		if (pos == std::string::npos)
			pos = tmpPath.size();
		buff = tmpPath.substr(slashPos, pos);
		tmpPath = tmpPath.substr(pos);
		if (buff == "/..")
			removeSegment(newPath);
		else if (buff == "/.")
			continue;
		else
			newPath += buff;
		
	}
	path = newPath;
}

bool isDirectory(const std::string& path) {
	struct stat st;
	return (stat(path.c_str(), &st) == 0 && S_ISDIR(st.st_mode));
}

std::string getExtension(const std::string& firstPath, const std::string& backUpPath) {
	std::string path;

	if (isDirectory(firstPath))
		path = backUpPath;
	else
		path = firstPath;

	size_t dotPos = path.find_last_of('.');
	std::string ext;

	ext = path.substr(dotPos);
	Exec _exec;
	std::string executable = _exec.getExec(ext);
	if (access(executable.c_str(), F_OK) != 0)
		return ""; 
	else if (access(executable.c_str(), X_OK) != 0)
		return "";
	return (executable);
}

bool isExtension(const std::string& firstPath, const std::string& backUpPath, std::vector<std::string> _ext) {
	std::string path;
	if (isDirectory(firstPath))
		path = backUpPath;
	else
		path = firstPath;
	size_t slashPos = path.find_last_of('/');
	size_t dotPos = path.find_last_of('.');
	std::string ext;

	if (dotPos == std::string::npos || (slashPos != std::string::npos && dotPos < slashPos))
		return false;
	ext = path.substr(dotPos);
	for (std::vector<std::string>::iterator it = _ext.begin(); it != _ext.end(); it++)
		if (*it == ext)
			return true;
	return false;
}

std::string methodToStr(HttpRequestMethod meth) {
	switch (meth)
	{
	case Get: return "GET";
		break;
	case Post: return "POST";
		break;
	case Delete: return "DELETE";
		break;
	default: return "Unsupported";
		break;
	}
}

bool isKeyValid(const std::string& line) {
	std::string tspecials("()<>@,;:\"/[]?={}");

	for (size_t i = 0; i < line.length(); i++) {
		if ((line[i] >= 0 && line[i] <= 32) || tspecials.find(line[i]) != std::string::npos || line[i] == 127)
			return false;
	}
	return true;
}

int hexCharToInt(char c) {
	c = std::toupper(c);
	if (c >= '0' && c <= '9')
		return c - '0';
	else if (c >= 'A' && c <= 'F')
		return c - 'A' + 10;
	return -1;
}

char hexToAscii(char a, char b) {
	int hi = hexCharToInt(std::toupper(a));
	int lo = hexCharToInt(std::toupper(b));
	if (hi == -1 || lo == -1)
		return 0;
	return static_cast<char>((hi << 4) | lo);
}

std::string MIME::getMIME(const std::string& extension) {
	if (_mime.find(extension) != _mime.end())
		return _mime[extension];
	return "application/octet-stream";
}

std::string Exec::getExec(std::string extension) {
	if (_exec.find(extension) != _exec.end())
		return _exec[extension];
	return "";
}


std::string REDIRECTS::getRedirectMsg(int code) {
	if (_redirect.find(code) != _redirect.end())
		return _redirect[code];
	return "";
}

std::string ERRORS::getErrorMsg(int errorCode) {
	if (_errors.find(errorCode) != _errors.end())
		return _errors[errorCode];
	return "HTTP/1.0 500 Internal Server Error\r\n";
}

std::string MIME::getContentExt(const std::string &ContentType)
{
	for (std::map<std::string, std::string>::iterator it = _mime.begin(); it != _mime.end(); it++) {
		if(it->second == ContentType)
			return (it->first);
	}
	return (".bin");
}

bool validcontentLength(std::string& contentlen) {
	for (size_t i = 0; i < contentlen.length(); i++) {
		if (!std::isdigit(contentlen[i]))
			return false;
	}
	return true;
}

std::string getFullPath(std::string root, std::string file) {
	size_t pos = file.find_first_not_of('/');

	if (pos != std::string::npos) {
		return root + file.substr(pos);
	}
	return root;
}

std::string removeRepeating(std::string str, const char &c)
{
  	std::string result;
    bool lastWasSlash = false;
    for (std::string::const_iterator it = str.begin(); it != str.end(); ++it) {
        if (*it == c) {
            if (!lastWasSlash) {
                result += *it;
                lastWasSlash = true;
            }
        } else {
            result += *it;
            lastWasSlash = false;
        }
    }
	if (result[result.size() - 1] == c)
		return result;
	return result + c;
}

std::string strToLower(const std::string& header) {
	std::string newHeader;
	for (size_t i = 0; i < header.length(); i++)
		newHeader += std::tolower(header[i]);
	return newHeader;
}
