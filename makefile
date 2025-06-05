# 终端匿名局域网聊天系统 Makefile
# 版本: 1.1.0
#By AndyChen(Andyccr)

# 基础配置
CXX ?= g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2
LDFLAGS = -lpthread

# 平台检测与设置
UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Linux)
    PLATFORM = linux
    TARGET_EXT =
    LDFLAGS += -lrt
endif
ifeq ($(UNAME_S),Darwin)
    PLATFORM = macos
    TARGET_EXT =
endif
ifeq ($(OS),Windows_NT)
    PLATFORM = windows
    TARGET_EXT = .exe
    LDFLAGS += -lws2_32
endif

# 目标设置
SERVER_TARGET = server_enhanced$(TARGET_EXT)
CLIENT_TARGET = client_enhanced$(TARGET_EXT)
TARGETS = $(SERVER_TARGET) $(CLIENT_TARGET)

# 调试模式设置
DEBUG ?= 0
ifeq ($(DEBUG),1)
    CXXFLAGS += -g -DDEBUG
endif

# 默认目标
all: $(TARGETS)

# 服务器构建
$(SERVER_TARGET): server_enhanced.cpp
	@echo "Building server for $(PLATFORM)..."
	$(CXX) $(CXXFLAGS) $< -o $@ $(LDFLAGS)
	@echo "Server build complete: $@"

# 客户端构建
$(CLIENT_TARGET): client_enhanced.cpp
	@echo "Building client for $(PLATFORM)..."
	$(CXX) $(CXXFLAGS) $< -o $@ $(LDFLAGS)
	@echo "Client build complete: $@"

# 清理
clean:
	@echo "Cleaning build files..."
	rm -f $(TARGETS)
	@echo "Clean complete."

# 安装到系统路径 (需要sudo)
install: all
	@echo "Installing to /usr/local/bin..."
	sudo cp $(TARGETS) /usr/local/bin/
	@echo "Installation complete."

# 卸载
uninstall:
	@echo "Uninstalling from /usr/local/bin..."
	sudo rm -f /usr/local/bin/$(SERVER_TARGET) /usr/local/bin/$(CLIENT_TARGET)
	@echo "Uninstall complete."

# 运行服务器 (开发模式)
run-server: $(SERVER_TARGET)
	@echo "Starting server..."
	./$(SERVER_TARGET) 8080

# 运行客户端 (开发模式)
run-client: $(CLIENT_TARGET)
	@echo "Starting client..."
	./$(CLIENT_TARGET) 127.0.0.1 8080 User$(shell date +%s)

# 帮助信息
help:
	@echo "终端匿名局域网聊天系统 Makefile 帮助"
	@echo ""
	@echo "可用命令:"
	@echo "  make all       - 构建服务器和客户端 (默认)"
	@echo "  make server    - 仅构建服务器"
	@echo "  make client    - 仅构建客户端"
	@echo "  make clean     - 清理构建文件"
	@echo "  make install   - 安装到系统路径"
	@echo "  make uninstall - 从系统卸载"
	@echo "  make run-server - 运行服务器 (端口8080)"
	@echo "  make run-client - 运行测试客户端"
	@echo ""
	@echo "调试选项:"
	@echo "  DEBUG=1 make   - 启用调试模式"

# 伪目标声明
.PHONY: all clean install uninstall run-server run-client help

# 别名定义
server: $(SERVER_TARGET)
client: $(CLIENT_TARGET)