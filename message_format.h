#ifndef MSG_FMT
#define MSG_FMT
#define MESSAGE_LEN 1020
#include <cstdlib>
#include <iostream>
using namespace std;
struct packet{
    int type;
    char* message;
};
// get byte array of struct
char* serialize(packet &msg)
{
    // Allocate 1024 bytes
    char* bytes = new char[1024];
    // Put in the type var first
    bytes[0] = msg.type & 0xFF;
    bytes[1] = (msg.type >> 8) & 0xFF;
    bytes[2] = (msg.type >> 16) & 0xFF;
    bytes[3] = (msg.type >> 24) & 0xFF;
    
    // Fill out remaining bytes as message
    for(int i = 4;i<MESSAGE_LEN+4;i++)
    {
        bytes[i] = msg.message[i-4];
    }
    // Return pointer
    return bytes;
}
// deserialize
packet* deserialize(char* bytes)
{
    packet* msg_packet = new packet();
    msg_packet->message = new char[MESSAGE_LEN];
    msg_packet->type = (unsigned int)(0x000000FF & bytes[0]) | (0x0000FF00 & (bytes[1] << 8)) | (0x00FF0000 & (bytes[2] << 16)) | (0xFF000000 & (bytes[3] << 24));
    // Fill out remaining bytes as message
    for(int i = 4;i<MESSAGE_LEN+4;i++)
    {
        msg_packet->message[i-4] = bytes[i];
    }
    // Return pointer
    return msg_packet;
}
#endif