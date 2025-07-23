#include "../../../includes/Request.hpp"

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
	std::string result;
		
	std::srand(std::time(NULL));
		
	for (size_t i = 0; i < 12; ++i) {
		result += characters[std::rand() % characters.size()];
	}
		
	return result;
}