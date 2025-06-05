#include <iostream>
#include <string>
#include <vector>
#include <thread>
#include <map>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>

const int BUFFER_SIZE = 1024;

struct ClientInfo {
    int socket;
    std::string username;
    std::string room;
};

std::vector<ClientInfo> clients;
std::mutex clients_mutex;

void broadcast_message(const std::string& message, const std::string& room, int exclude_socket = -1) {
    std::lock_guard<std::mutex> lock(clients_mutex);
    for (const auto& client : clients) {
        if (client.room == room && client.socket != exclude_socket) {
            send(client.socket, message.c_str(), message.size(), 0);
        }
    }
}

void handle_client(int client_socket) {
    char buffer[BUFFER_SIZE];
    ClientInfo current_client{client_socket, "", ""};
    
    // 获取用户名和房间
    int bytes_received = recv(client_socket, buffer, BUFFER_SIZE, 0);
    if (bytes_received <= 0) {
        close(client_socket);
        return;
    }
    buffer[bytes_received] = '\0';
    
    std::string init_data(buffer);
    size_t delimiter = init_data.find(':');
    if (delimiter == std::string::npos) {
        close(client_socket);
        return;
    }
    
    current_client.username = init_data.substr(0, delimiter);
    current_client.room = init_data.substr(delimiter + 1);
    
    {
        std::lock_guard<std::mutex> lock(clients_mutex);
        clients.push_back(current_client);
    }
    
    std::string welcome_msg = "[系统] " + current_client.username + " 加入了房间 " + current_client.room + "\n";
    broadcast_message(welcome_msg, current_client.room);
    
    while (true) {
        bytes_received = recv(client_socket, buffer, BUFFER_SIZE, 0);
        if (bytes_received <= 0) break;
        
        buffer[bytes_received] = '\0';
        std::string message = "[" + current_client.username + "] " + buffer;
        broadcast_message(message, current_client.room);
    }
    
    // 客户端断开连接
    {
        std::lock_guard<std::mutex> lock(clients_mutex);
        clients.erase(std::remove_if(clients.begin(), clients.end(), 
            [client_socket](const ClientInfo& c) { return c.socket == client_socket; }), 
            clients.end());
    }
    
    std::string leave_msg = "[系统] " + current_client.username + " 离开了房间\n";
    broadcast_message(leave_msg, current_client.room);
    
    close(client_socket);
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <port>" << std::endl;
        return 1;
    }

    int server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket < 0) {
        std::cerr << "Socket creation failed" << std::endl;
        return 1;
    }

    sockaddr_in server_address{};
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(std::stoi(argv[1]));
    server_address.sin_addr.s_addr = INADDR_ANY;

    if (bind(server_socket, (sockaddr*)&server_address, sizeof(server_address)) < 0) {
        std::cerr << "Bind failed" << std::endl;
        return 1;
    }

    if (listen(server_socket, 5) < 0) {
        std::cerr << "Listen failed" << std::endl;
        return 1;
    }
    
    std::cout << "Server listening on port " << argv[1] << std::endl;

    while (true) {
        sockaddr_in client_address{};
        socklen_t client_len = sizeof(client_address);
        int client_socket = accept(server_socket, (sockaddr*)&client_address, &client_len);
        
        if (client_socket < 0) {
            std::cerr << "Accept failed" << std::endl;
            continue;
        }
        
        char client_ip[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &client_address.sin_addr, client_ip, INET_ADDRSTRLEN);
        std::cout << "New connection from " << client_ip << std::endl;
        
        std::thread(handle_client, client_socket).detach();
    }

    close(server_socket);
    return 0;
}