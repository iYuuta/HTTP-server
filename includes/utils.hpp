#ifndef UTILS_HPP
#define UTILS_HPP

# include <iostream>
# include <string>
# include <fstream>
# include <sstream>
# include <vector>
# include <iterator>
# include <map>
# include <ctime>
# include <cerrno>
# include <climits>
# include <cstdlib>
# include <sys/stat.h>
# include <unistd.h>
# include <fcntl.h>
# include <sys/wait.h>
# include <dirent.h>
# include "HttpRequestMethod.hpp"
# include "Location.hpp"

#define BUFFER_SIZE 100000

#define ERROR_PAGE_START "<!DOCTYPE html>\n" \
"<html lang=\"en\">\n" \
"<head>\n" \
"  <meta charset=\"UTF-8\">\n" \
"  <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n" \
"  <title>Error</title>\n" \
"  <style>\n" \
"    body { font-family: Arial, sans-serif; background-color: #121212; color: #f5f5f5; text-align: center; padding: 50px; }\n" \
"    .container { max-width: 600px; margin: auto; background: #1e1e1e; border-radius: 10px; padding: 30px; box-shadow: 0 0 15px rgba(0,0,0,0.5); }\n" \
"    h1 { font-size: 2.5em; color: #ff4c4c; margin-bottom: 20px; }\n" \
"    p { font-size: 1.2em; margin-bottom: 10px; color: #ddd; }\n" \
"    .footer { margin-top: 30px; font-size: 0.9em; color: #999; }\n" \
"  </style>\n" \
"</head>\n" \
"<body>\n" \
"  <div class=\"container\">\n"

#define ERROR_PAGE_END "<div class=\"footer\">&copy; 1337 yuuta</div>\n  </div>\n</body>\n</html>\n"

enum  enums {
	NOTHING,
	REQUESLINE,
	HEADERS,
	STATUSLINE_HEADERS,
	BODY,
	READ,
	WRITE,
	BUILT,
	DONE,
	CLOSED
};

struct MIME {
	private:
		std::map<std::string, std::string> _mime;

	public:
		MIME();
		std::string getMIME(const std::string& extension);
		std::string getContentExt(const std::string &ContentType);
};

struct ERRORS {
	private:
		std::map<int, std::string> _errors;

	public:
		ERRORS();
		std::string getErrorMsg(int errorCode);
};

struct Exec {
	private:
		std::map<std::string, std::string> _exec;

	public:
		Exec();
		std::string getExec(std::string extension);
};


std::vector<std::string> split(const std::string& s, char delimiter);
std::vector<std::string> splitNumber(const std::string& s);
unsigned long atoiul(const std::string& s);
std::string intToString(int n);
std::string getContentType(const std::string& fileName);
std::string trim(const std::string& s);
std::string generateRandomName();
bool isDirectory(const std::string& path);
bool isExtension(const std::string& path, std::vector<std::string> _ext);
std::string getExtension(const std::string& path);
std::string methodToStr(HttpRequestMethod meth);
bool isKeyValid(const std::string& line);
char hexToAscii(char a, char b);
std::string joinUrlPaths(const std::string &firstPath, const std::string &secondPath);

#endif
