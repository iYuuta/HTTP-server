#include "../../includes/Config.hpp"
#include <sys/socket.h>

#include <iostream>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>

void Server::tempserver(void) {
    int server_fd, client_fd;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = inet_addr("127.0.0.1");
    address.sin_port = htons(8080);
    if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0) {
        throw 1;
    }
    listen(server_fd, 5);
    std::cout << "listening on 127.0.0.1:8080\n";
    while (true) {
    client_fd = accept(server_fd, (struct sockaddr*)&address, (socklen_t*)&addrlen);
    if (client_fd > 0) {
        _clients[client_fd] = new Client(_locations, _maxAllowedClientRequestSize, client_fd);

        char buffer[BUFFER_SIZE];
        int bytes_read;
        bool request_done = false;
        while (!request_done && (bytes_read = read(client_fd, buffer, sizeof(buffer))) > 0) {
            _clients[client_fd]->getData(buffer, bytes_read);
            if (_clients[client_fd]->isRequestDone()) {
                request_done = true;
            }
        }
        close(client_fd);
        delete _clients[client_fd];
        _clients.erase(client_fd);
    }
}
    close(server_fd);
}
