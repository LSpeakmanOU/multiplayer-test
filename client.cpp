#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>
#include <strings.h>
#include "message_format.h"
#include "game_context.h"

using namespace std;

int main(int argc, char* argv[]){
    packet datasend;
    player_data my_data;
    datasend.message = (char*)malloc(MESSAGE_LEN);
    bzero(datasend.message, MESSAGE_LEN);
    int c_fd = socket(AF_INET, SOCK_STREAM, 0);
    if(c_fd == -1){
        perror("socket");
        exit(EXIT_FAILURE);
    }
    char buffer[1024] = { 0 };
    ssize_t buff_msg_size;

    string hello = "Hello from client";
    sockaddr_in server;
    server.sin_family = AF_INET;
    //server.sin_addr.s_addr = inet_addr("127.0.0.1");???

    server.sin_port = htons(25565);
    //inet_pton(AF_INET, "127.0.0.1", &server.sin_addr)
    int status = connect(c_fd, (struct sockaddr*)&server, sizeof(server));
    if(status == -1){
        perror("connect");
        exit(EXIT_FAILURE);
    }
    string input;
    // First ask user what their name is upon logging in
    datasend.type = INTRO;
    cout << "What is your name?" << endl;
    getline(cin, input);
    strncpy(datasend.message, input.c_str(), input.length());
    char* serialized_msg = serialize(datasend);
    int s_val = send(c_fd, serialized_msg, 1024, 0);
    if(s_val == -1){
        perror("send");
        exit(EXIT_FAILURE);
    }
    delete[] serialized_msg;
    my_data.name = input;
    getline(cin, input);
    while(input != "exit"){
        datasend.type = SAY;
        // Zero message container, fill it with the input, send it
        bzero(datasend.message, MESSAGE_LEN);
        strncpy(datasend.message, input.c_str(), input.length());
        char* serialized_msg = serialize(datasend);
        int s_val = send(c_fd, serialized_msg, 1024, 0);
        if(s_val == -1){
            perror("send");
            exit(EXIT_FAILURE);
        }
        delete[] serialized_msg;
        bzero(buffer, 1024); // zero buffer
        buff_msg_size = read(c_fd, buffer, 1024);
        if(buff_msg_size == -1){
            perror("read");
            exit(EXIT_FAILURE);
        }
        cout << buffer << endl;
        getline(cin, input);
    }
    cout << "Goodbye!" << endl;
    // closing the connected socket
    close(c_fd);
    return EXIT_SUCCESS;
}