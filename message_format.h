#ifndef MSG_FMT_H
#define MSG_FMT_H
#define MESSAGE_LEN 1016
#include <cstdlib>
#include <iostream>
using namespace std;
enum packet_type{
    SAY,
    INTRO,
    GOODBYE,
    GOODBYE_ACK,
    SERVER_GREETING,
};
struct packet{
    int type;
    int from; // only to be used by server-->client traffic
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

    bytes[4] = msg.from & 0xFF;
    bytes[5] = (msg.from >> 8) & 0xFF;
    bytes[6] = (msg.from >> 16) & 0xFF;
    bytes[7] = (msg.from >> 24) & 0xFF;

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
    msg_packet->type = (unsigned int)(0x000000FF & bytes[0]) | (0x0000FF00 & (bytes[1] << 8)) | (0x00FF0000 & (bytes[2] << 16)) | (0xFF000000 & (bytes[3] << 24));
    msg_packet->from = (unsigned int)(0x000000FF & bytes[4]) | (0x0000FF00 & (bytes[5] << 8)) | (0x00FF0000 & (bytes[6] << 16)) | (0xFF000000 & (bytes[7] << 24));

    // Fill out remaining bytes as message
    for(int i = 8;i<MESSAGE_LEN+8;i++)
    {
        msg_packet->message[i-8] = bytes[i];
    }
    // Return pointer
    return msg_packet;
}
#endif