// client_std.cpp - 使用标准库的TCP客户端
#include <iostream>
#include <string>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <thread>

const int BUFFER_SIZE = 1024;

void receive_messages(int socket) {
    char buffer[BUFFER_SIZE];
    while (true) {
        int bytes_received = recv(socket, buffer, BUFFER_SIZE, 0);
        if (bytes_received <= 0) {
            std::cerr << "Connection closed by server" << std::endl;
            break;
        }
        buffer[bytes_received] = '\0';
        std::cout << "Server response: " << buffer << std::endl;
    }
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <server_ip> <server_port>" << std::endl;
        return 1;
    }

    // 创建socket
    int client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket < 0) {
        std::cerr << "Socket creation failed" << std::endl;
        return 1;
    }

    // 配置服务器地址
    sockaddr_in server_address{};
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(std::stoi(argv[2]));
    
    if (inet_pton(AF_INET, argv[1], &server_address.sin_addr) <= 0) {
        std::cerr << "Invalid address/Address not supported" << std::endl;
        return 1;
    }

    // 连接服务器
    if (connect(client_socket, (sockaddr*)&server_address, sizeof(server_address)) < 0) {
        std::cerr << "Connection failed" << std::endl;
        return 1;
    }

    std::cout << "Connected to server. Type messages to send (type 'exit' to quit):" << std::endl;

    // 启动接收线程
    std::thread receiver(receive_messages, client_socket);
    receiver.detach();

    // 主线程处理用户输入
    std::string message;
    while (true) {
        std::getline(std::cin, message);
        if (message == "exit") break;
        
        if (send(client_socket, message.c_str(), message.size(), 0) < 0) {
            std::cerr << "Message send failed" << std::endl;
            break;
        }
    }

    close(client_socket);
    return 0;
}