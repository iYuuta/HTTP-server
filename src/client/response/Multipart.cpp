#include "../../../includes/Response.hpp"

bool Response::readFileToString(const std::string& filePath, std::string& content) {
    std::ifstream fileStream(filePath.c_str(), std::ios::in | std::ios::binary);
    if (!fileStream.is_open()) {
        _errorCode = 500;
        return (false);
    }

    char buffer[BUFFER_SIZE];
    content.clear();
    while (fileStream.read(buffer, sizeof(buffer)))
        content.append(buffer, sizeof(buffer));
    content.append(buffer, fileStream.gcount());

    fileStream.close();
    return (true);
}

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

void Response::processPartBody(Multipart& part, const std::string& bodyContent) {
    if (part.isFile) {
        part.tempFilePath = generateRandomName();
        std::ofstream tempFileStream(part.tempFilePath.c_str(), std::ios::binary | std::ios::out);
        if (!tempFileStream.is_open()) {
            _errorCode = 500;
            throw (std::string) "Failed to create temporary file for upload.";
        }
        tempFileStream.write(bodyContent.data(), bodyContent.length());
        tempFileStream.close();
    } else {
        part.contentBody = bodyContent;
    }
}

void Response::parseMultipartBody() {
    std::string rawBodyContent;

    if (!readFileToString(_request.getFileName(), rawBodyContent))
        throw (std::string) "Failed to read request body";

    if (!extractBoundary(_request.getHeader("Content-Type"), _boundary))
        throw (std::string) "multipart error invalid boundary.";

    std::string delimiter = "--" + _boundary;
    std::string finalDelimiter = delimiter + "--";
    size_t currentPos = rawBodyContent.find(delimiter);

    if (currentPos == std::string::npos) {
        _errorCode = 400;
        throw (std::string) "multipart error no initial boundary.";
    }
    currentPos += delimiter.length();
    if (rawBodyContent.substr(currentPos, 2) == "\r\n") currentPos += 2;

    while (true) {
        size_t nextDelimiterPos = rawBodyContent.find(delimiter, currentPos);
        if (nextDelimiterPos == std::string::npos) {
            _errorCode = 400;
            throw (std::string) "multipart error no closing boundary.";
        }

        std::string partContent = rawBodyContent.substr(currentPos, nextDelimiterPos - currentPos);
        if (partContent.length() > 1 && partContent.substr(partContent.length() - 2) == "\r\n") {
             partContent.erase(partContent.length() - 2);
        }

        size_t headerEndPos = partContent.find("\r\n\r\n");
        if (headerEndPos == std::string::npos) {
            _errorCode = 400;
            throw (std::string) "multipart error headers not terminated.";
        }

        std::string headerStr = partContent.substr(0, headerEndPos);
        std::string bodyStr = partContent.substr(headerEndPos + 4);

        Multipart currentPart;
        parsePartHeaders(headerStr, currentPart);
        processPartBody(currentPart, bodyStr);
        _multiparts.push_back(currentPart);

        if (rawBodyContent.substr(nextDelimiterPos, finalDelimiter.length()) == finalDelimiter)
            break;

        currentPos = nextDelimiterPos + delimiter.length();
        if (rawBodyContent.substr(currentPos, 2) == "\r\n") currentPos += 2;
    }
}