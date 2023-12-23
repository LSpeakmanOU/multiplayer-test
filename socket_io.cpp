#include "socket_io.h"
namespace SocketIO{
    int create_client_socket(){
        int c_fd = socket(AF_INET, SOCK_STREAM, 0);
        if(c_fd == -1){
            perror("socket");
            exit(EXIT_FAILURE);
        }
        sockaddr_in server;
        server.sin_family = AF_INET;
        server.sin_addr.s_addr = inet_addr("127.0.0.1");

        server.sin_port = htons(25565);
        int status = connect(c_fd, (struct sockaddr*)&server, sizeof(server));
        if(status == -1){
            perror("connect");
            exit(EXIT_FAILURE);
        }
        return c_fd;
    }
    int create_server_socket(sockaddr_in &temp_address,socklen_t &addrlen){
        int s_fd = socket(AF_INET, SOCK_STREAM, 0);
        
        const int enable = 1;
        setsockopt(s_fd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(enable)); // allow to bind to TIME_WAIT port
        if(s_fd == -1){
            perror("socket");
            exit(EXIT_FAILURE);
        }
        temp_address.sin_family = AF_INET;
        temp_address.sin_addr.s_addr = inet_addr("127.0.0.1");
        temp_address.sin_port = htons(25565);
         int b_val = bind(s_fd, (sockaddr *)&temp_address, sizeof(temp_address));
        if(b_val == -1){
            perror("bind");
            exit(EXIT_FAILURE);
        }
        int l_val = listen(s_fd, 10); // 3 simultaneous connection queue
        if(l_val == -1){
            perror("listen");
            exit(EXIT_FAILURE);
        }
        return s_fd;
    }
    void close_client_socket(int c_fd){
        // closing the connected socket
        close(c_fd);
    }
    bool recv_msg(int fd, char* buffer, int buff_len){
        ssize_t buff_msg_size = read(fd, buffer, 1024);
        if(buff_msg_size == -1){
            perror("read");
            exit(EXIT_FAILURE);
        }
        return buff_msg_size > 0;
    }
    void send_msg(int to_fd, packet &msg){
        char* msg_to_send = serialize(msg);
        int send_val = send(to_fd, msg_to_send, 1024, 0);
        if(send_val == -1){
            perror("send");
            exit(EXIT_FAILURE);
        }
        delete[] msg_to_send;
    }
    void broadcast(int source_fd, const vector<int> &clients, packet* msg){
        char* msg_to_send = serialize(*msg);
        for(int i = 0;i<clients.size();i++){
            if(clients[i] != source_fd){
                int send_val = send(clients[i], msg_to_send, 1024, 0);
                if(send_val == -1){
                    perror("send");
                    exit(EXIT_FAILURE);
                }
            }
        }   
        delete[] msg_to_send;
    }
    // assuming length 4 array starting with the address of the first byte
    void serialize_int(char* bytes, int val){
        bytes[0] = val & 0xFF;
        bytes[1] = (val >> 8) & 0xFF;
        bytes[2] = (val >> 16) & 0xFF;
        bytes[3] = (val >> 24) & 0xFF;
    }
    // assuming length 4 array starting with the address of the first byte
    int deserialize_int(char* bytes){
        return (unsigned int)(0x000000FF & bytes[0]) | (0x0000FF00 & (bytes[1] << 8)) | (0x00FF0000 & (bytes[2] << 16)) | (0xFF000000 & (bytes[3] << 24));
    }
    // get byte array of struct
    char* serialize(packet &msg)
    {
        // Allocate 1024 bytes
        char* bytes = new char[1024];
        // Add type and from information
        serialize_int(bytes, msg.type);
        serialize_int(&bytes[4], msg.from);

        // Fill out remaining bytes as message
        for(int i = 8;i<MESSAGE_LEN+8;i++)
        {
            bytes[i] = msg.message[i-8];
        }
        // Return pointer
        return bytes;
    }
    // deserialize
    packet* deserialize(char* bytes)
    {
        packet* msg_packet = new packet();
        msg_packet->message = new char[MESSAGE_LEN];
        msg_packet->type = deserialize_int(bytes);
        msg_packet->from = deserialize_int(&bytes[4]);
    
        // Fill out remaining bytes as message
        for(int i = 8;i<MESSAGE_LEN+8;i++)
        {
            msg_packet->message[i-8] = bytes[i];
        }
        // Return pointer
        return msg_packet;
    }
}