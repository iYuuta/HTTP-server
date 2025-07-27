#include "../../includes/Client.hpp"

Client::Client(const int& fd, Server& server, std::map<int, std::string>& errorp):
												_server(server),
												_location(server.getLocations().begin()),
												_errorPages(errorp),
												_fd(fd),
												_errorCode(-1),
												_responseDone(false),
												_requestDone(false),
												_activeCgi(false),
												request(),
												response(request, _server.getErrorPages(), _location)
{
}

std::ostream& operator<<(std::ostream& os, Request& req);

Client::~Client()
{
}

void Client::parseRequest()
{
	char buffer[BUFFER_SIZE];
	ssize_t len;

	len = read(_fd, buffer, BUFFER_SIZE);
	if (len < 0)
		throw (std::string) "read error";
	try {
		request.parseData(buffer, len);
		if (request.getParseState() == DONE)
		{
			_requestDone = true;
			// std::cout << request << std::endl;
			if (!isRequestValid()) {
				response.setErrorCode(_errorCode);
				return ;
			}
		}
	}
	catch (std::string error) {
		if (_errorCode)
			response.setErrorCode(_errorCode);
		_requestDone = true;
		std::cerr << "Error: " << error << std::endl;
	}
}

void Client::createResponse() {
	response.buildResponse();
}

void Client::writeData() {
	// std::cout << "write response\n";
	const std::string& buff = response.getResponse();
	write(_fd, buff.c_str(), buff.size());
}

bool Client::isRequestDone() {
	return _requestDone;
}

bool Client::isResponseDone() {
	return _responseDone;
}

bool Client::isFinished() {
	if (response.getResponseState() == DONE)
		return true;
	return false;
}

std::ostream& operator<<(std::ostream& os, Request& req)
{
	os << "=== HTTP Request ===\n";

	switch (req.getMeth())
	{
	case Get: os << "Method: GET\n";
		break;
	case Post: os << "Method: POST\n";
		break;
	case Delete: os << "Method: DELETE\n";
		break;
	default: os << "Method: Unsupported\n";
		break;
	}

	os << "Path: " << req.getPath() << "\n";
	os << "Version: " << req.getVersion() << "\n";

	os << "\nHeaders:\n";
	for (std::map<std::string, std::string>::iterator it = req.getHeaders().begin(); it != req.getHeaders().end(); it++)
	{
		os << it->first << ": " << it->second << "\n";
	}

	if (req.getContentLen() > 0)
	{
		os << "\nBody File Content:\n";
		os << req.getBodyFile().rdbuf();
		os << "\nContent-Length: " << req.getContentLen() << "\n";
		os << "Received Bytes: " << req.getReceivedBytes() << "\n";
	}
	os << "====================\n";
	return os;
}
