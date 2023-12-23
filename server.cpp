#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <thread>
#include <sys/time.h>
#include <vector>
#include <strings.h>
#include <string.h>
#include "message_format.h"
#include "game_context.h"
#include "socket_io.h"

using namespace std;

void handle_disconnect(int fd, int fd_idx, sockaddr_in &temp_address, socklen_t &addrlen, vector<int> &clients, vector<player_data> &players){
    getpeername(fd, (struct sockaddr*)&temp_address, (socklen_t*)&addrlen); 
    cout << "Client Disconnected: " << inet_ntoa(temp_address.sin_addr) << ":" << ntohs(temp_address.sin_port) << endl;
    close(fd);
    clients.erase(clients.begin()+fd_idx);
    // Delete player data
    players.erase(players.begin()+fd_idx);
}

int main(int argc, char* argv[]){
    sockaddr_in temp_address;
    socklen_t addrlen = sizeof(temp_address);
    fd_set readfds;
    char buffer[1024] = { 0 };
    bool msg_size_gzero;
    string hello = "Hello from server";
    int new_c_fd;
    vector<int> clients;
    vector<player_data> players;
    packet datasend;
    datasend.message = (char*)malloc(MESSAGE_LEN);
    bzero(datasend.message, MESSAGE_LEN);
    int s_fd = SocketIO::create_server_socket(temp_address, addrlen);
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
            // Send welcome message + player list
            cout << "Client Connected: " << inet_ntoa(temp_address.sin_addr) << ":" << ntohs(temp_address.sin_port) << endl;
            for(int i = 0;i < clients.size();i++){
                datasend.type = INTRO_MSG;
                datasend.from = clients[i];
                bzero(datasend.message, MESSAGE_LEN);
                strncpy(datasend.message, players[i].name.c_str(), players[i].name.length());
                SocketIO::send_msg(new_c_fd, datasend);
            }
            // Add to client FD list and add new player data object
            clients.push_back(new_c_fd);
            player_data temp_pd = player_data();
            players.push_back(temp_pd);
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
                msg_size_gzero = SocketIO::recv_msg(curr_fd, buffer, 1024);
                // Handle message if length is greater than zero, otherwise the client disconnected
                if(msg_size_gzero){
                    // Deserialize message, process it, then delete it
                    packet* new_msg = SocketIO::deserialize(buffer);
                    switch(new_msg->type){
                        case SAY_MSG:
                            cout << players[i].name << ": ";
                            for(int i = 0;i<MESSAGE_LEN;i++)
                                cout << new_msg->message[i];
                            cout << endl;
                            new_msg->from = curr_fd;
                            SocketIO::broadcast(curr_fd, clients, new_msg);
                            break;
                        case INTRO_MSG:
                            players[i].name = string(new_msg->message);
                            new_msg->from = curr_fd;
                            SocketIO::broadcast(curr_fd, clients, new_msg);
                            break;
                        case GOODBYE_MSG:
                            // Acknowledge player's exit message
                            // Used in killing player's application
                            new_msg->type = GOODBYE_ACK_MSG;
                            new_msg->from = curr_fd;
                            SocketIO::send_msg(curr_fd, *new_msg);
                            break;
                        case ENTER_MSG:
                            // Update player's location
                            players[i].location = SocketIO::deserialize_int(new_msg->message);
                            // Update all players' location information for current player
                            new_msg->from = curr_fd;
                            SocketIO::broadcast(curr_fd, clients, new_msg);
                        default:
                            break;
                    }
                    // Delete deserialized object
                    delete[] new_msg->message;
                    delete new_msg;
                    
                }
                if(!msg_size_gzero){
                    handle_disconnect(curr_fd, i, temp_address, addrlen, clients, players);
                    bzero(datasend.message, MESSAGE_LEN);
                    // Send goodbye message to all players
                    datasend.type = GOODBYE_MSG;
                    datasend.from = curr_fd;
                    SocketIO::broadcast(curr_fd, clients, &datasend);
                }
            }
        }
    }
    close(s_fd);
    delete[] datasend.message;

    return EXIT_SUCCESS;
}