#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <thread>
#include "message_format.h"

using namespace std;

int main(int argc, char* argv[]){
    int s_fd = socket(AF_INET, SOCK_STREAM, 0);
    const int enable = 1;
    setsockopt(s_fd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(enable)); // allow to bind to TIME_WAIT port
    if(s_fd == -1){
        perror("socket");
        exit(EXIT_FAILURE);
    }
    char buffer[1024] = { 0 };
    ssize_t buff_msg_size;
    string hello = "Hello from server";

    int new_c_fd;
    sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = inet_addr("127.0.0.1");
	server.sin_port = htons(25565);
    socklen_t serverlen = sizeof(server);
    int b_val = bind(s_fd, (sockaddr *)&server, sizeof(sockaddr_in));
    if(b_val == -1){
        perror("bind");
        exit(EXIT_FAILURE);
    }
    int l_val = listen(s_fd, 3); // 3 simultaneous connection queue
    if(l_val == -1){
        perror("listen");
        exit(EXIT_FAILURE);
    }
    new_c_fd = accept(s_fd, (sockaddr *)&server, &serverlen);
    if(new_c_fd == -1){
        perror("accept");
        exit(EXIT_FAILURE);
    }
    buff_msg_size = read(new_c_fd, buffer, 1024 - 1);// -1 for null terminator
    if(buff_msg_size == -1){
        perror("read");
        exit(EXIT_FAILURE);
    }
    packet* new_msg = deserialize(buffer);
    cout << (unsigned int)new_msg->type << endl;
    for(int i = 0;i<MESSAGE_LEN;i++)
        cout << new_msg->message[i];
    cout << endl;
    int s_val = send(new_c_fd, hello.c_str(), hello.length(), 0);
    if(s_val == -1){
        perror("send");
        exit(EXIT_FAILURE);
    }
    cout << "message sent" << endl;
    close(new_c_fd);
    close(s_fd);
    return EXIT_SUCCESS;
}