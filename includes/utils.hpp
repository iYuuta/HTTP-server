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

#define BUFFER_SIZE 4096

enum  enums {
	NOTHING,
	REQUESLINE,
	HEADERS,
	STATUSLINE_HEADERS,
	BODY,
	READ,
	WRITE,
	DONE,
	CLOSED
};

std::vector<std::string> split(const std::string& s, char delimiter);
std::vector<std::string> splitNumber(const std::string& s);
unsigned long atoiul(const std::string& s);
std::string intToString(int n);
std::string getContentType(const std::string& fileName);
std::string trim(const std::string& s);
std::string generateRandomName();
bool isRegularFile(const std::string& path);
bool isDirectory(const std::string& path);
bool isExtension(const std::string& path, std::vector<std::string> _ext);
std::string getExtension(const std::string& path);
std::string methodToStr(HttpRequestMethod meth);
bool isKeyValid(const std::string& line);
char hexToAscii(char a, char b);

#endif
