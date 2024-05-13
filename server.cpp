#include <sys/socket.h>
#include <netinet/in.h>
#include <iostream>
#include <thread>
#include <unistd.h>
#include <arpa/inet.h>
#include <shared_mutex>
/*
127.0.0.1:9527/?query=<query string>
curl -i "http://127.0.0.1:9527/?query=aaabbbbcc"
response = "HTTP/1.1 200 OK\r\n"
           "Content-Type: text/html; charset=utf-8\r\n"
           "\r\n"
           "<html><head><title>Get Request</title></head><body><h1>Get Request Received!</h1></body></html>";
*/

#include "CommandExecutor.hpp"

static CommandExecutor Executor("data.csv");
static QueryCommandParser QueryParser;
static ModifyCommandParser ModifyParser;
std::shared_mutex io_mtx;

enum ServerType {
    QUERY,
    MODIFICATION
};

// 处理HTTP请求
std::string handleRequest(const std::string &method, const std::string &uri, const std::string &body, ServerType type) {
    std::string response;

    if (method == "GET") {
        // 处理GET请求
        std::string command = uri.substr(uri.find_first_of('=') + 1);
        // std::cout << "Command: " << command << std::endl;
        if (type == QUERY) {
            std::vector<std::string> results;
            QueryCommandContent query_command;
            std::string msg;
            if (!QueryParser.Parse(command, query_command, msg)) {
                msg = "{\"msg\": \"" + msg + "\"}";
                response = "HTTP/1.1 400 Bad Request\r\n"
                        "Content-Type: text/html; charset=utf-8\r\n"
                        "\r\n"
                        +msg;
                return response;
            }
            bool ret = false;
            {
                std::shared_lock<std::shared_mutex> lck(io_mtx);
                ret = Executor.Execute(query_command, results, msg);
            }           
            if (!ret) {
                msg = "{\"msg\": \"" + msg + "\"}";
                response = "HTTP/1.1 400 Bad Request\r\n"
                        "Content-Type: text/html; charset=utf-8\r\n"
                        "\r\n"
                        +msg;
                return response;
            }
            response = "HTTP/1.1 200 OK\r\n"
                    "Content-Type: text/html; charset=utf-8\r\n"
                    "\r\n";
            for (int i = 0; i < results.size(); i++) {
                response += results[i];
                if (i != results.size() - 1) {
                    response += "\n";
                }
            }
        } else if (type == MODIFICATION) {
            ModifyCommandContent modify_command;
            std::string msg;
            if (!ModifyParser.Parse(command, modify_command, msg)) {
                msg = "{\"msg\": \"" + msg + "\"}";
                response = "HTTP/1.1 400 Bad Request\r\n"
                        "Content-Type: text/html; charset=utf-8\r\n"
                        "\r\n"
                        +msg;
                return response;
            }
            bool ret = false;
            {
                std::unique_lock<std::shared_mutex> lck(io_mtx);
                ret = Executor.Execute(modify_command, msg);
            }           
            if (!ret) {
                msg = "{\"msg\": \"" + msg + "\"}";
                response = "HTTP/1.1 400 Bad Request\r\n"
                        "Content-Type: text/html; charset=utf-8\r\n"
                        "\r\n"
                        +msg;
                return response;
            }
            response = "HTTP/1.1 200 OK\r\n"
                    "Content-Type: text/html; charset=utf-8\r\n"
                    "\r\n"
                    +msg;
        }
    } else {
        // 其他请求
        response = "HTTP/1.1 404 Not Found\r\n"
                   "Content-Type: text/html; charset=utf-8\r\n"
                   "\r\n"
                   "<html><head><title>Only accept get request</title></head><body><h1>404 Not Found</h1></body></html>";
    }
    return response;
}

// 处理客户端连接
void handleClient(int client_socket, ServerType type) {
    char buffer[1024];

    while (true) {
        // 接收HTTP请求
        std::string request;
        int n = recv(client_socket, buffer, sizeof(buffer), 0);
        if (n <= 0) {
            break;
        }
        request.append(buffer, n);
        // std::cout << request << std::endl;

        // 解析HTTP请求
        size_t pos = request.find(' ');
        if (pos == std::string::npos) {
            continue;
        }
        std::string method = request.substr(0, pos);
        // std::cout << "Request Method: " << method << std::endl;

        pos = request.find(' ', pos + 1);
        if (pos == std::string::npos) {
            continue;
        }
        std::string uri = request.substr(request.find(' ') + 1, pos - request.find(' ') - 1);
        // std::cout << "Request URI: " << uri << std::endl;

        // 查找HTTP请求头结束位置
        pos = request.find("\r\n\r\n");
        if (pos == std::string::npos) {
            continue;
        }

        // 解析HTTP请求Body
        std::string body = request.substr(pos + 4);
        // std::cout << "Request Body: " << body << std::endl;

        // 处理HTTP请求
        std::string response = handleRequest(method, uri, body, type);
        std::cout << "Response: " << response << std::endl;

        // 发送HTTP响应
        send(client_socket, response.c_str(), response.length(), 0);

        // 关闭连接
        close(client_socket);
        break;
    }
}

bool CreateServer(ServerType type) {
    int server_socket;
    struct sockaddr_in server_addr;

    server_socket = socket(AF_INET, SOCK_STREAM, 0);

    int on = 1;
    setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));

    if (server_socket < 0) { 
        std::cerr << "create socket error"; 
        return false;
    }

    int port;
    if (type == QUERY) {
        port = 9527;
    } else if (type == MODIFICATION) {
        port = 7259;
    }
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family       = AF_INET;
    server_addr.sin_port         = htons(port);
    server_addr.sin_addr.s_addr  = htonl(INADDR_ANY);

    if (bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) { 
        std::cerr << "bind address error"; 
        return false;
    }

    if (listen(server_socket, 8) < 0) { 
        std::cerr << "listen port error"; 
        return false;
    }

    while (true) {
        // 接收连接请求
        struct sockaddr_in client_address;
        socklen_t client_address_len = sizeof(client_address);
        int client_socket = accept(server_socket, (struct sockaddr *) &client_address, &client_address_len);
        if (client_socket == -1) {
            std::cerr << "Accept failed!" << std::endl;
            continue;
        }

        std::cout << "Client connected. IP address: " << inet_ntoa(client_address.sin_addr) << std::endl;

        // 处理客户端连接
        std::thread client_thread(handleClient, client_socket, type);
        client_thread.detach();
    }

    close(server_socket);
    return true;
}

int main(int argc, char* argv[]) 
{
    std::thread query_server(CreateServer, QUERY);
    std::thread modification_server(CreateServer, MODIFICATION);
    query_server.join();
    modification_server.join();
    return 0;
}