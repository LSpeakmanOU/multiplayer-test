#ifndef CONTEXT_H
#define CONTEXT_H
#include <string>
using namespace std;
enum locations{
    SPAWN,
    BAZAAR,
};
struct player_data{
    string name;
    int location = SPAWN;
};
#endif