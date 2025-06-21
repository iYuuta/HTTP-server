#ifndef UTILS_HPP
#define UTILS_HPP

# include <iostream>
# include <fstream>
# include <vector>
# include <sstream>

bool operator>>(std::ifstream &in, std::string &out);
std::vector<std::string> split(const std::string& s);

#endif
