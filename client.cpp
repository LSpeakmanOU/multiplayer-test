#include <iostream>
#include <cstring> // strncpy
#include <thread>
#include <mutex>
#include <map>
#include <vector>
#include <stdlib.h>
#include "message_format.h"
#include "game_context.h"
#include "socket_io.h"
#define LINUX

using namespace std;
bool Running = true;
std::mutex mtx; 

void increase_msg_log(vector<string> &message_log, string target){
    mtx.lock();
    message_log.push_back(target);
    cout << target;
    mtx.unlock();
}
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
void listen_to_server_traffic(int c_fd, map<int, player_data> &players, vector<string> &message_log){
    char buffer[1024] = { 0 };
    bool msg_size_gzero;
    string temp_string;
    while(Running){
        bzero(buffer, 1024); // zero buffer
        msg_size_gzero = SocketIO::recv_msg(c_fd, buffer, 1024);
        // Handle message if length is greater than zero, otherwise the client disconnected
        if(msg_size_gzero){
            // Deserialize message, process it, then delete it
            packet* new_msg = SocketIO::deserialize(buffer);
            switch(new_msg->type){
            case SAY_MSG:
                temp_string = "";
                temp_string += players[new_msg->from].name + ": ";
                for(int i = 0;i<MESSAGE_LEN;i++)
                    temp_string += new_msg->message[i];
                temp_string += "\n";
                increase_msg_log(message_log, temp_string);
                break;
            case INTRO_MSG:
                players[new_msg->from] = player_data();
                //players[new_msg->from] = new player_data;
                players[new_msg->from].name = string(new_msg->message);
                break;
            case ENTER_MSG:
                players[new_msg->from].location = SocketIO::deserialize_int(new_msg->message);
                break;
            case GOODBYE_MSG: // SOMEONE ELSE
                temp_string = "";
                temp_string += "Player: " + players[new_msg->from].name + " Has disconnected!\n";
                increase_msg_log(message_log, temp_string);
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
    vector<string> message_log;
    map<int, player_data> players;
    packet datasend;
    player_data my_data;
    datasend.message = (char*)malloc(MESSAGE_LEN);
    bzero(datasend.message, MESSAGE_LEN);
    
    int c_fd = SocketIO::create_client_socket();
    thread server_listener(listen_to_server_traffic, c_fd, ref(players), ref(message_log));
    string input;
    // First ask user what their name is upon logging in
    datasend.type = INTRO_MSG;
    cout << "What is your name?" << endl;
    getline(cin, input);
    strncpy(datasend.message, input.c_str(), input.length());
    SocketIO::send_msg(c_fd,datasend);
    my_data.name = input;
    increase_msg_log(message_log, "Welcome to <NAME> " + my_data.name + "!\n");
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
                SocketIO::send_msg(c_fd, datasend);
                increase_msg_log(message_log, my_data.name + ": " + temp_string + "\n");
                break;
            case INSPECT_ACTION:
            {
                if(tokens.size() == 1)
                    continue;
                temp_string = input.substr(input.find(" ") + 1);
                vector<string> inspects = get_inspects(my_data.location);
                for(int i = 0; i<inspects.size();i++){
                    if(temp_string == inspects[i]){
                        increase_msg_log(message_log, inspect(temp_string));
                    }
                }
                break;
            }
            case ENTER_ACTION:
            {
                if(tokens.size() == 1)
                    break;
                temp_string = input.substr(input.find(" ") + 1);
                int loc_id = get_loc_id(temp_string);
                if(loc_id == INVALID_LOCATION)
                    break;
                my_data.location = loc_id;
                datasend.type = ENTER_MSG;
                bzero(datasend.message, MESSAGE_LEN);
                SocketIO::serialize_int(temp_byte_arr, loc_id);
                // Send 1 int over as the message(location)
                strncpy(datasend.message, temp_byte_arr, 4);
                SocketIO::send_msg(c_fd, datasend);
                break;
            }
            case LOOK_ACTION:
                increase_msg_log(message_log, look(my_data.location));
                break;
            case HELP_ACTION:
                increase_msg_log(message_log, help());
                break;
            case WHO_ACTION:
                increase_msg_log(message_log, who(players));
                break;
            case INVALID_ACTION:
            default:
                break;
        }
        getline(cin, input);
    }
    datasend.type = GOODBYE_MSG;
    bzero(datasend.message, MESSAGE_LEN);
    SocketIO::send_msg(c_fd, datasend);
    
    delete[] datasend.message;
    cout << "Goodbye!" << endl;
    Running=false;
    server_listener.join();
    SocketIO::close_client_socket(c_fd);
    return EXIT_SUCCESS;
}