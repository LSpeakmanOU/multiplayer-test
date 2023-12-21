#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <thread>
#include <sys/time.h>
#include <vector>
#include "message_format.h"
#include <strings.h>
using namespace std;

int main(int argc, char* argv[]){
    int s_fd = socket(AF_INET, SOCK_STREAM, 0);
    fd_set readfds;
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
    sockaddr_in temp_address;
    socklen_t addrlen = sizeof(temp_address);

    temp_address.sin_family = AF_INET;
    temp_address.sin_addr.s_addr = inet_addr("127.0.0.1");
	temp_address.sin_port = htons(25565);
    vector<int> clients;
    int b_val = bind(s_fd, (sockaddr *)&temp_address, sizeof(temp_address));
    if(b_val == -1){
        perror("bind");
        exit(EXIT_FAILURE);
    }
    int l_val = listen(s_fd, 3); // 3 simultaneous connection queue
    if(l_val == -1){
        perror("listen");
        exit(EXIT_FAILURE);
    }
    int max_fd;
    int curr_fd;
    int active_fds;
    while(1){
        // Clear socket set
        FD_ZERO(&readfds);
        //add master socket
        FD_SET(s_fd, &readfds);
        max_fd = s_fd;
        // Add other client sockets if valid and update max_fd
        for(int i = 0;i < clients.size();i++){
            curr_fd = clients[i];
            if(curr_fd > 0)
                FD_SET(curr_fd, &readfds);
            if(curr_fd > max_fd)
                max_fd = curr_fd;
        }
        // Begin waiting for active file descriptors
        // First argument = highest numbered FD in set + 1
        active_fds = select(max_fd + 1, &readfds, NULL, NULL, NULL);
        if(active_fds == -1){
            perror("select");
            exit(EXIT_FAILURE);
        }
        // Check master socket for new connection
        if(FD_ISSET(s_fd, &readfds)){
            // Accept new connection
            new_c_fd = accept(s_fd, (sockaddr *)&temp_address, &addrlen);
            if(new_c_fd == -1){
                perror("accept");
                exit(EXIT_FAILURE);
            }
            // Send welcome message
            cout << "Client Connected: " << inet_ntoa(temp_address.sin_addr) << ":" << ntohs(temp_address.sin_port) << endl;
            int send_val = send(new_c_fd, hello.c_str(), hello.length(), 0);
            if(send_val == -1){
                perror("send");
                exit(EXIT_FAILURE);
            }
            // Add to client FD list
            clients.push_back(new_c_fd);
        }
        // If its not the master socket, its a client
        for(int i = 0;i<clients.size();i++)
        {
            curr_fd = clients[i];
            // Check for if FD is ready
            // 2 cases, closing and IO
            if(FD_ISSET(curr_fd, &readfds)){
                // Unwrap message, if size = 0 then you are closing, otherwise its a message
                bzero(buffer, 1024); // zero buffer
                buff_msg_size = read(new_c_fd, buffer, 1024);
                if(buff_msg_size == -1){
                    perror("read");
                    exit(EXIT_FAILURE);
                }
                // If closing, remove client
                // Otherwise, handle message
                if(buff_msg_size > 0){
                    packet* new_msg = deserialize(buffer);
                    cout << (unsigned int)new_msg->type << endl;
                    for(int i = 0;i<MESSAGE_LEN;i++)
                        cout << new_msg->message[i];
                    cout << endl;
                }
                if(buff_msg_size == 0 ){
                    getpeername(curr_fd, (struct sockaddr*)&temp_address, (socklen_t*)&addrlen); 
                    cout << "Client Disconnected: " << inet_ntoa(temp_address.sin_addr) << ":" << ntohs(temp_address.sin_port) << endl;
                    close(curr_fd);
                    clients.erase(clients.begin()+i);
                }
            }
        }
    }
    close(s_fd);
    return EXIT_SUCCESS;
}