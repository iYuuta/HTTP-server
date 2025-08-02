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
            std::string name = trim(line.substr(0, colonPos));
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
    }
}

void Response::parseMultipartBody() {
    if (!extractBoundary(_request.getHeader("Content-Type"), _boundary)) {
        _errorCode = 400;
        throw std::runtime_error("Multipart error: Invalid or missing boundary.");
    }
    std::string boundary_delimiter = "--" + _boundary;
    std::string header_separator = "\r\n\r\n";

    std::ifstream body_stream(_request.getFileName().c_str(), std::ios::binary);
    if (!body_stream.is_open()) {
        _errorCode = 500;
        throw std::runtime_error("Failed to open request body.");
    }

    std::string buffer;
    char chunk[BUFFER_SIZE];
    ParseState state = LOOKING_FOR_START_BOUNDARY;
    Multipart currentPart;
    std::ofstream out_stream;

    bool has_more_data = true;
    while (has_more_data && state != FINISHED) {
        if (!body_stream.eof() && body_stream.good()) {
            body_stream.read(chunk, sizeof(chunk));
            buffer.append(chunk, body_stream.gcount());
        } else
            has_more_data = false;

        bool state_has_changed = true;
        while (state_has_changed) {
            state_has_changed = false;
            switch (state) {
                case LOOKING_FOR_START_BOUNDARY: {
                    size_t pos = buffer.find(boundary_delimiter + "\r\n");
                    if (pos != std::string::npos) {
                        buffer.erase(0, pos + boundary_delimiter.length() + 2);
                        state = PARSING_HEADERS;
                        state_has_changed = true;
                    }
                    break;
                }
                case PARSING_HEADERS: {
                    size_t pos = buffer.find(header_separator);
                    if (pos != std::string::npos) {
                        std::string headers_str = buffer.substr(0, pos);
                        buffer.erase(0, pos + header_separator.length());
                        
                        currentPart = Multipart();
                        parsePartHeaders(headers_str, currentPart);

                        if (currentPart.isFile) {
                            currentPart.tempFilePath = generateRandomName();
                            out_stream.open(currentPart.tempFilePath.c_str(), std::ios::binary);
                            if (!out_stream.is_open()) {
                                _errorCode = 500;
                                throw std::runtime_error("Failed to create temporary file for upload.");
                            }
                        }
                        state = STREAMING_BODY;
                        state_has_changed = true;
                    }
                    break;
                }
                case STREAMING_BODY: {
                    std::string end_delimiter = "\r\n" + boundary_delimiter;
                    size_t pos = buffer.find(end_delimiter);
                    if (pos != std::string::npos) {
                        if (out_stream.is_open()) {
                            out_stream.write(buffer.c_str(), pos);
                            out_stream.close();
                        }
                        _multiparts.push_back(currentPart);
                        buffer.erase(0, pos + end_delimiter.length());
                        if (buffer.rfind("--", 0) == 0)
                            state = FINISHED;
                        else if (buffer.rfind("\r\n", 0) == 0) {
                            buffer.erase(0, 2);
                            state = PARSING_HEADERS;
                        } else
                            throw std::runtime_error("Multipart error: Malformed boundary.");
                        state_has_changed = true;
                    }
                    break;
                }
                case FINISHED:
                    break;
            }
        }
    }
    if (state != FINISHED) {
        if (out_stream.is_open())
            out_stream.close();
        throw std::runtime_error("Multipart error: Incomplete or malformed request body.");
    }
}