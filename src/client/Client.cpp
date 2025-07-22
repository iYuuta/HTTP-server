#include "../../includes/Client.hpp"

Client::Client(std::vector<Location>* locations, size_t MaxRequestSize, int fd): _fd(fd), _MaxRequestSize(MaxRequestSize), _response_done(false), _request_done(false), _ActiveCgi(false), _error_code(-1), request(fd){}

std::ostream& operator<<(std::ostream& os, Request& req);

Client::~Client() {}

void Client::GetData(char *buffer, size_t len) {
	request.ParseData(buffer, len);
	if (request.GetParseState() == DONE) {
		_request_done = true;
		std::cout << request << std::endl;
	}
}

bool	Client::IsRequestDone() {
	return _request_done;
}


std::ostream& operator<<(std::ostream& os, Request& req) {
    os << "=== HTTP Request ===\n";

    // Method
    switch (req.GetMeth()) {
        case GET: os << "Method: GET\n"; break;
        case POST: os << "Method: POST\n"; break;
        case DELETE: os << "Method: DELETE\n"; break;
        default: os << "Method: Unsupported\n"; break;
    }

    os << "FD: " << req.GetFd() << "\n";
    os << "Path: " << req.GetPath() << "\n";
    os << "Version: " << req.GetVersion() << "\n";

    os << "\nHeaders:\n";
    for (const auto& header : req.GetHeaders()) {
        os << header.first << ": " << header.second << "\n";
    }

    os << "\nBody File Content:\n";
    os << req.GetBodyFile().rdbuf();

	os << "\nContent-Length: " << req.GetContentLen() << "\n";
	os << "Received Bytes: " << req.GetReceivedBytes() << "\n";
    os << "====================\n";
    return os;
}