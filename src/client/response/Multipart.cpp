#include "../../../includes/Response.hpp"

bool Response::extractBoundary(const std::string& contentTypeHeader, std::string& boundary) {
	size_t boundaryPos = contentTypeHeader.find("boundary=");
	if (boundaryPos == std::string::npos) {
		_errorCode = 400;
		return (false);
	}
	boundary = contentTypeHeader.substr(boundaryPos + 9);
	if (boundary.length() > 1 && boundary[0] == '"' && boundary[boundary.length() - 1] == '"')
		boundary = boundary.substr(1, boundary.length() - 2);
	return (true);
}

void Response::parsePartHeaders(const std::string& headerStr, Multipart& part) {
	std::istringstream headerStream(headerStr);
	std::string line;

	while (std::getline(headerStream, line)) {
		if (!line.empty() && line[line.length() - 1] == '\r')
			line.erase(line.length() - 1);

		size_t colonPos = line.find(':');
		if (colonPos != std::string::npos) {
			std::string name = line.substr(0, colonPos);
			if (!isKeyValid(name)) {
				_errorCode = 400;
				throw std::string("Bad request");
			}
			std::string value = trim(line.substr(colonPos + 1));

			part.headers[name] = value;
			if (name == "Content-Disposition") {
				size_t namePos = value.find("name=\"");
				if (namePos != std::string::npos) {
					size_t nameEnd = value.find("\"", namePos + 6);
					if (nameEnd != std::string::npos)
						part.contentDispositionName = value.substr(namePos + 6, nameEnd - (namePos + 6));
				}
				size_t filenamePos = value.find("filename=\"");
				if (filenamePos != std::string::npos) {
					size_t filenameEnd = value.find("\"", filenamePos + 10);
					if (filenameEnd != std::string::npos) {
						part.contentDispositionFilename = value.substr(filenamePos + 10, filenameEnd - (filenamePos + 10));
						part.isFile = true;
					}
				}
			}
			else if (name == "Content-Type")
				part.contentType = value;
		}
		else {
			if (line.empty() || (line[0] != ' ' && line[0] != '\t')) {
				_errorCode = 400;
				throw std::string("Bad request");
			}
			if (part.headers.empty()) {
				_errorCode = 400;
				throw std::string("Bad request");
			}
			if (!part.headers.empty())
				part.headers.rbegin()->second = part.headers.rbegin()->second + " " + trim(line);

		}
	}
}

void Response::parseMultipartBody(const std::string& uploadPath) {
	if (!extractBoundary(_request.getHeader("Content-Type"), _boundary)) {
		_errorCode = 400;
		throw std::runtime_error("Invalid or missing boundary.");
	}

	std::string start_boundary = "--" + _boundary;
	std::string end_boundary = start_boundary + "--";
	std::string header_separator = "\r\n\r\n";

	std::ifstream body_stream(_request.getFileName().c_str(), std::ios::binary);
	if (!body_stream.is_open()) {
		_errorCode = 500;
		throw std::runtime_error("Failed to open request body.");
	}

	char chunk[BUFFER_SIZE];
	std::string buffer;
	ParseState state = LOOKING_FOR_START_BOUNDARY;
	Multipart currentPart;
	std::ofstream file_stream;

	while (state == LOOKING_FOR_START_BOUNDARY) {
		body_stream.read(chunk, sizeof(chunk));
		size_t bytes_read = body_stream.gcount();
		if (bytes_read == 0)
			throw std::runtime_error("Start boundary not found.");

		buffer.append(chunk, bytes_read);

		size_t pos = buffer.find(start_boundary);
		if (pos != std::string::npos) {
			buffer.erase(0, pos + start_boundary.length());
			if (buffer.find("\r\n") == 0)
				buffer.erase(0, 2);

			state = PARSING_HEADERS;
		}
	}

	while (state != FINISHED) {

		if (buffer.length() < (BUFFER_SIZE * 2) && body_stream) {
			body_stream.read(chunk, sizeof(chunk));
			buffer.append(chunk, body_stream.gcount());
		}

		if (state == PARSING_HEADERS) {
			size_t pos = buffer.find(header_separator);
			if (pos != std::string::npos) {
				std::string headers_str = buffer.substr(0, pos);
				buffer.erase(0, pos + header_separator.length());

				currentPart = Multipart();
				parsePartHeaders(headers_str, currentPart);

				if (currentPart.isFile && !currentPart.contentDispositionFilename.empty()) {
					std::string finalFilePath = uploadPath + "/" + currentPart.contentDispositionFilename;
					file_stream.open(finalFilePath.c_str(), std::ios::binary);
					if (!file_stream.is_open()) {
						_errorCode = 500;
						throw std::runtime_error("Failed to create file for upload.");
					}
				}
				state = STREAMING_BODY;
			} else if (!body_stream) 
				throw std::runtime_error("Multipart error: Malformed headers.");
		}

		if (state == STREAMING_BODY) {
			std::string boundary_delimiter = "\r\n" + start_boundary;
			size_t pos = buffer.find(boundary_delimiter);

			if (pos != std::string::npos) {
				if (file_stream.is_open()) {
					file_stream.write(buffer.c_str(), pos);
					file_stream.close();
				}
			
				buffer.erase(0, pos + boundary_delimiter.length());

				if (buffer.find("--") == 0)
					state = FINISHED;
				else if (buffer.find("\r\n") == 0) {
					buffer.erase(0, 2);
					state = PARSING_HEADERS;
				}

			} else {

				size_t tail_size = start_boundary.length() + 4;
				if (buffer.length() > tail_size) {
					size_t write_size = buffer.length() - tail_size;
					if (file_stream.is_open())
						file_stream.write(buffer.c_str(), write_size);

					buffer.erase(0, write_size);
				} else if (!body_stream) {
					if (file_stream.is_open()) {
						file_stream.write(buffer.c_str(), buffer.length());
						file_stream.close();
					}
					state = FINISHED;
				}
			}
		}
	}
}