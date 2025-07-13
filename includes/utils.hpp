#ifndef UTILS_HPP
#define UTILS_HPP

# include <iostream>
# include <fstream>
# include <vector>
# include <sstream>

bool operator>>(std::ifstream &in, std::string &out);
int indexOf(std::string arr[], size_t arrSize, std::string toFind);
std::vector<std::string> split(const std::string& s, char delimiter);
std::vector<std::string> splitNumber(const std::string& s);

#endif
