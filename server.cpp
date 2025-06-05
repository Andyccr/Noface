// server_std.cpp - 使用标准库的替代方案
#include <iostream>
#include <string>
#include <vector>
#include <thread>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>

const int BUFFER_SIZE = 1024;

void handle_client(int client_socket) {
    char buffer[BUFFER_SIZE];
    while (true) {
        int bytes_received = recv(client_socket, buffer, BUFFER_SIZE, 0);
        if (bytes_received <= 0) break;
        
        buffer[bytes_received] = '\0';
        std::cout << "Received: " << buffer << std::endl;
        
        std::string response = "Server received: " + std::string(buffer);
        send(client_socket, response.c_str(), response.size(), 0);
    }
    close(client_socket);
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <port>" << std::endl;
        return 1;
    }

    int server_socket = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in server_address{};
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(std::stoi(argv[1]));
    server_address.sin_addr.s_addr = INADDR_ANY;

    bind(server_socket, (sockaddr*)&server_address, sizeof(server_address));
    listen(server_socket, 5);
    
    std::cout << "Server listening on port " << argv[1] << std::endl;

    while (true) {
        sockaddr_in client_address{};
        socklen_t client_len = sizeof(client_address);
        int client_socket = accept(server_socket, (sockaddr*)&client_address, &client_len);
        
        std::thread(handle_client, client_socket).detach();
    }

    close(server_socket);
    return 0;
}