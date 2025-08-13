#include "../../includes/Client.hpp"

Client::Client(const int& fd, Server& server, std::map<int, std::string>& errorp):
												_server(server),
												_location(server.getLocations().begin()),
												_errorPages(errorp),
												_fd(fd),
												_errorCode(-1),
												_responseDone(false),
												_requestDone(false),
												request(),
												response(request, _server.getErrorPages(), _location)
{
}

Client::~Client()
{
}

std::ostream& operator<<(std::ostream& os, Request& req);

void Client::parseRequest()
{
	char buffer[BUFFER_SIZE];
	ssize_t len;

	len = read(_fd, buffer, BUFFER_SIZE);
	if (len < 0) {
		_requestDone = true;
		response.setErrorCode(500);
		return;
	}
	buffer[len] = '\0';
	try {
		request.parseData(buffer, len);
		if (request.getParseState() == DONE)
		{
			_requestDone = true;
			if (!isRequestValid()) {
				response.setErrorCode(_errorCode);
				return ;
			}
		}
	}
	catch (std::string error) {
		response.setErrorCode(request.getErrorCode());
		_requestDone = true;
		std::cerr << "Error: " << error << std::endl;
	}
}

void Client::createResponse() {
	response.buildResponse();
}

void Client::writeData() {
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
