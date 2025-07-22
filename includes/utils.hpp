#ifndef UTILS_HPP
#define UTILS_HPP

# include <iostream>
# include <fstream>
# include <vector>
# include <sstream>
# include <cstdlib>

enum  enums {
	NOTHING,
	GET,
	DELETE,
	POST,
	UNSUPPORTED,
	REQUESLINE,
	HEADERS,
	BODY,
	READ,
	WRITE,
	DONE,
	CLOSED
};

std::vector<std::string> split(const std::string& s, char delimiter);
std::vector<std::string> splitNumber(const std::string& s);
unsigned long atoiul(const std::string& s);
#endif
