#ifndef SOCKET_IO_H
#define SOCKET_IO_H
#include <sys/socket.h>
#include <vector>
#include "message_format.h"
using namespace std;
namespace SocketIO{
    void send_msg(int to_fd, packet &msg);
    void broadcast(int source_fd, const vector<int> &clients, packet* msg);
    void serialize_int(char* bytes, int val);
    int deserialize_int(char* bytes);
    char* serialize(packet &msg);
    packet* deserialize(char* bytes);
};
#endif