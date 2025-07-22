#ifndef UTILS_HPP
#define UTILS_HPP

# include <iostream>
# include <fstream>
# include <vector>
# include <sstream>
# include <cstdlib>

enum  enums {
	nothing,
	Get,
	Delete,
	Post,
	Unsupported,
	RequestLine,
	Headers,
	Body,
	Read,
	Write,
	Done,
	Closed
};

std::vector<std::string> split(const std::string& s, char delimiter);
std::vector<std::string> splitNumber(const std::string& s);
unsigned long atoiul(const std::string& s);
#endif
