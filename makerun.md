g++ -std=c++17 server_std.cpp -o server -lpthread

g++ -std=c++17 client_std.cpp -o client -lpthread
./server 8080
./client 127.0.0.1 8080

