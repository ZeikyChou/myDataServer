#include <sys/socket.h>
#include <netinet/in.h>
#include <iostream>
#include <thread>
#include <unistd.h>
#include <arpa/inet.h>


int main() {
    char* msg = "Hello, World!";
    int client_socket = socket(AF_INET, SOCK_STREAM, 0);
    int on = 1;
    setsockopt(client_socket, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
    if (client_socket < 0) {
        std::cerr << "create socket error";
        return -1;
    }

    //向服务器（特定的IP和端口）发起请求
    struct sockaddr_in serv_addr;
    memset(&serv_addr, 0, sizeof(serv_addr));  //每个字节都用0填充
    serv_addr.sin_family = AF_INET;  //使用IPv4地址
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");  //具体的IP地址
    serv_addr.sin_port = htons(9527);  //端口
    if (connect(client_socket, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        std::cerr << "connect error";
        return -1;
    }
    send(client_socket, msg, strlen(msg), 0);
    close(client_socket);
    return 0;
}