#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>
#include <strings.h>
#include <thread>
#include <map>
#include <vector>
#include "message_format.h"
#include "game_context.h"

using namespace std;
bool Running = true;
vector<string> collect_tokens(const string &input_str){
    vector<string> result;
    string curr_str = input_str;
    std::size_t found = curr_str.find(" ");
    while(found != string::npos){
        result.push_back(curr_str.substr(0, found));
        curr_str = curr_str.substr(found+1);
        found = curr_str.find(" ");
    }
    result.push_back(curr_str);
    return result;
}
void listen_to_server_traffic(int c_fd, map<int, player_data> &players){
    char buffer[1024] = { 0 };
    ssize_t buff_msg_size;
    while(Running){
        bzero(buffer, 1024); // zero buffer
        buff_msg_size = read(c_fd, buffer, 1024);
        if(buff_msg_size == -1){
            perror("read");
            exit(EXIT_FAILURE);
        }
        // Handle message if length is greater than zero, otherwise the client disconnected
        if(buff_msg_size > 0){
        // Deserialize message, process it, then delete it
        packet* new_msg = deserialize(buffer);
        switch(new_msg->type){
        case SAY_MSG:
            cout << players[new_msg->from].name << ": ";
            for(int i = 0;i<MESSAGE_LEN;i++)
                cout << new_msg->message[i];
                cout << endl;
                break;
        case INTRO_MSG:
            players[new_msg->from] = player_data();
            //players[new_msg->from] = new player_data;
            players[new_msg->from].name = string(new_msg->message);
            break;
        case ENTER_MSG:
            players[new_msg->from].location = deserialize_int(new_msg->message);
            break;
        case GOODBYE_MSG: // SOMEONE ELSE
            cout << "Player: " << players[new_msg->from].name << " Has disconnected!" << endl;
            players.erase(new_msg->from);
            break;
        case GOODBYE_ACK_MSG: // END THREAD
            return;
        default:
            break;
        }
        // Delete deserialized object
        delete[] new_msg->message;
        delete new_msg;
        }
    }
}
int main(int argc, char* argv[]){
    map<int, player_data> players;
    packet datasend;
    player_data my_data;
    datasend.message = (char*)malloc(MESSAGE_LEN);
    bzero(datasend.message, MESSAGE_LEN);
    int c_fd = socket(AF_INET, SOCK_STREAM, 0);
    if(c_fd == -1){
        perror("socket");
        exit(EXIT_FAILURE);
    }

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
    thread server_listener(listen_to_server_traffic, c_fd, ref(players));
    string input;
    // First ask user what their name is upon logging in
    datasend.type = INTRO_MSG;
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
    string temp_string;
    char temp_byte_arr[4];
    while(input != "exit"){
        vector<string> tokens = collect_tokens(input);
        int action_selected = get_action(tokens[0]);
        switch(action_selected){
            case SAY_ACTION:
                if(tokens.size() == 1)
                    continue;
                temp_string = input.substr(input.find(" ") + 1);
                datasend.type = SAY_MSG;
                // Zero message container, fill it with the input, send it
                bzero(datasend.message, MESSAGE_LEN);
                strncpy(datasend.message, temp_string.c_str(), temp_string.length());
                send_msg(c_fd, datasend);
                break;
            case INSPECT_ACTION:
                break;
            case ENTER_ACTION:
            {
                if(tokens.size() == 1)
                    break;
                temp_string = input.substr(input.find(" ") + 1);
                int loc_temp = get_location(temp_string);
                if(loc_temp == INVALID_LOCATION)
                    break;
                datasend.type = ENTER_MSG;
                bzero(datasend.message, MESSAGE_LEN);
                serialize_int(temp_byte_arr, loc_temp);
                // Send 1 int over as the message(location)
                strncpy(datasend.message, temp_byte_arr, 4);
                send_msg(c_fd, datasend);
                break;
            }
            case LOOK_ACTION:
                break;
            case HELP_ACTION:
                help();
                break;
            case WHO_ACTION:
                who(players);
                break;
            case INVALID_ACTION:
            default:
                break;
        }
        
        getline(cin, input);
    }
    datasend.type = GOODBYE_MSG;
    bzero(datasend.message, MESSAGE_LEN);
    serialized_msg = serialize(datasend);
    s_val = send(c_fd, serialized_msg, 1024, 0);
    if(s_val == -1){
        perror("send");
        exit(EXIT_FAILURE);
    }
    delete[] serialized_msg;
    delete[] datasend.message;
    cout << "Goodbye!" << endl;
    Running=false;
    server_listener.join();
    // closing the connected socket
    close(c_fd);
    return EXIT_SUCCESS;
}