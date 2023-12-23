#ifndef MSG_FMT_H
#define MSG_FMT_H
#define MESSAGE_LEN 1016
#include <cstdlib>
#include <iostream>
using namespace std;
enum packet_type{
    SAY_MSG,
    INTRO_MSG,
    GOODBYE_MSG,
    GOODBYE_ACK_MSG,
    SERVER_GREETING_MSG,
    ENTER_MSG
};
struct packet{
    int type;
    int from; // only to be used by server-->client traffic
    char* message;
};
#endif