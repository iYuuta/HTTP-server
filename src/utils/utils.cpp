# include "../../includes/utils.hpp"

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

unsigned long atoiul(const std::string& s)
{
	errno = 0;
	char* ptr;
	const unsigned long value = std::strtoul(s.c_str(), &ptr, 10);
	if (errno != 0 || *ptr != '\0')
		throw std::invalid_argument(s);
	return (value);
}

std::string getContentType(const std::string& fileName) {
	size_t pos = fileName.find_last_of('.');
	if (pos == std::string::npos)
		return "application/octet-stream";

	std::string ext = fileName.substr(pos);

	if (ext == ".html" || ext == ".htm")
		return "text/html";
	else if (ext == ".css")
		return "text/css";
	else if (ext == ".js")
		return "application/javascript";
	else if (ext == ".json")
		return "application/json";
	else if (ext == ".png")
		return "image/png";
	else if (ext == ".jpg")
		return "image/jpg";
	else if (ext == ".gif")
		return "image/gif";
	else if (ext == ".svg")
		return "image/svg+xml";
	else if (ext == ".ico")
		return "image/x-icon";
	else if (ext == ".txt")
		return "text/plain";
	else if (ext == ".pdf")
		return "application/pdf";
	else
		return "application/octet-stream";
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
	std::string result;
	for (size_t i = 0; i < 12; ++i)
		result += characters[std::rand() % characters.size()];
	return "/tmp/" + result;
}

bool isDirectory(const std::string& path) {
	struct stat st;
	return (stat(path.c_str(), &st) == 0 && S_ISDIR(st.st_mode));
}

bool isRegularFile(const std::string& path) {
	struct stat st;
	return (stat(path.c_str(), &st) == 0 && S_ISREG(st.st_mode));
}

std::string getExtension(const std::string& path) {
	size_t dotPos = path.find_last_of('.');
	std::string ext;

	ext = path.substr(dotPos);
	if (ext == ".py")
		return "/usr/bin/python3";
	else if (ext == ".php")
		return "/usr/bin/php";
	else if (ext == ".sh")
		return "/bin/bash";
	return "";
}

bool isExtension(const std::string& path) {
	size_t slashPos = path.find_last_of('/');
	size_t dotPos = path.find_last_of('.');
	std::string ext;

	if (dotPos == std::string::npos || (slashPos != std::string::npos && dotPos < slashPos))
		return false;
	ext = path.substr(dotPos);
	if (ext == ".py" || ext == ".php" || ext == ".sh")
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
    int hi = hexCharToInt(toupper(a));
    int lo = hexCharToInt(toupper(b));
    if (hi == -1 || lo == -1)
        return 0;
    return static_cast<char>((hi << 4) | lo);
}