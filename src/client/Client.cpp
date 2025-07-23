#include "../../includes/Client.hpp"

Client::Client(std::vector<Location> locations, size_t maxRequestSize, int fd): _fd(fd), _maxRequestSize(maxRequestSize), _responseDone(false), _requestDone(false), _activeCgi(false), _errorCode(-1), _locations(locations), request(fd, _locations) {}

std::ostream& operator<<(std::ostream& os, Request& req);

Client::~Client() {}

void Client::getData(char *buffer, size_t len) {
	request.parseData(buffer, len);
	if (request.getParseState() == DONE) {
        _requestDone = true;
		std::cout << request << std::endl;
        if (request.isTargetValid())
            request.isMethodValid();
	}
}

bool	Client::isRequestDone() {
	return _requestDone;
}


std::ostream& operator<<(std::ostream& os, Request& req) {
    os << "=== HTTP Request ===\n";

    switch (req.getMeth()) {
        case GET: os << "Method: GET\n"; break;
        case POST: os << "Method: POST\n"; break;
        case DELETE: os << "Method: DELETE\n"; break;
        default: os << "Method: Unsupported\n"; break;
    }

    os << "Path: " << req.getPath() << "\n";
    os << "Version: " << req.getVersion() << "\n";

    os << "\nHeaders:\n";
    for (const auto& header : req.getHeaders()) {
        os << header.first << ": " << header.second << "\n";
    }

    if (req.getContentLen() > 0) {
        os << "\nBody File Content:\n";
        os << req.getBodyFile().rdbuf();
	    os << "\nContent-Length: " << req.getContentLen() << "\n";
	    os << "Received Bytes: " << req.getReceivedBytes() << "\n";
    }
    os << "====================\n";
    return os;
}