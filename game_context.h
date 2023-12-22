#ifndef CONTEXT_H
#define CONTEXT_H
#include <string>
#include <map>

using namespace std;

enum locations{
    INVALID_LOCATION,
    VILLAGE,
    MAGE_STORE,
};
enum action_type{
    INVALID_ACTION,
    SAY_ACTION,
    INSPECT_ACTION,
    ENTER_ACTION,
    LOOK_ACTION,
    HELP_ACTION,
    WHO_ACTION,
};
struct player_data{
    string name;
    int location = VILLAGE;
};

string get_location(int loc);
int get_loc_id(string loc);
int get_action(string loc);
void who(const map<int, player_data> &players);
void help();


const string village_overview = "A small village with a mage shop.";
const vector<string> village_options{"mage store"};
const vector<string> village_inspects{"mage store"};
const string mage_store_overview = "A small, quaint mage shop stands before you. The building features wooden "
"supports amidst white mortar walls and a somewhat haphazard stone base; however, the most "
"striking element of this store is the frequent stained glass windows of various color combinations littered "
"around the various walls. Inside, the windows illuminate overflowing store shelves of various complementary "
"colored potions and crystals. You notice that the back wall is lined with bookcases of grimoires and arcane "
"symbols and diagrams plastered to the walls. The store clerk is stout, indigo-colored elderly goblin-folk. "
"She beckons you over as soon as you enter. \"Welcome! Let me know if you need any help lookin' around\"";
const vector<string> mage_store_options{"village"};
const vector<string> mage_store_inspects = {"mage store","orb", "wand", "robe"};
const vector<string> magic_colors = {"red", "blue", "green", "yellow", "black", "white", "purple"};
const vector<string> magic_adjectives = {"dull", "vibrant", "glowing", "dark", "light"};
const vector<string> magic_trim_materials = {"gold", "steel", "silver", "iron"};
const vector<string> magic_trims = {"chain links", "leaves and vines", "stitching", "embroidery"};
const char MAGIC_OPTIONS = 3;
const string orb_dialogue[MAGIC_OPTIONS] = {
    "As you approach the orb, an almost alarming heat radiates towards you along with a vibrant orange-yellow "
    "light. Swirling flames dance around the inside of the orb. It almost appears as if the orb is breathing "
    "as it harmonically brightens and dims. While initially taken aback, there is something comforting about" 
    "it. It reminds you of a warm day lounging in a field in the sunlight.",
    "As you approach the orb, you feel lighter and feel a small amount of vertigo. The orb seems to distort the "
    "gravitational forces on those who observe it. You look into the orb and you see purple, cloudy arms spiraling "
    "out from a bright, pinkish center with what looks like stars littering the center. Despite the orb's size, "
    "it feels incredibly large as if you were looking into the edges of space.",
    "Approaching the orb, you feel yourself becoming intertwined with the essence of everything around you. From "
    "the pulsing of the blood through your body down to the smallest creatures in the dirt below the shop. You "
    "feel your clarity in the energies around you increase as you approach the orb. Staring at the glass sphere, "
    "the orb potrays a scene of opalescent shimmers amidst a pearl-esc background."
};
const string wand_dialogue[MAGIC_OPTIONS] = {
    "Passing through the aisle of wands, a redwood wand vibrates with energy and emits yellow sparks. You hear "
    "thunder softly crackling from the wand. A loose wisp of energy flies out of the wand past your face. You "
    "briefly smell Bergamot and lemongrass.",
    "As you walk by their selection of wands, you brush a spruce wand with your hand. Your hand is suddenly cold "
    "and the wand begins to make sound similar to wind chimes. Arctic wind flows forth from the tip of the wand "
    "for a short time but not before freezing a vial of moon water on the windowsill.",
    "On your way through the wands, an oak wand calls to you. The whispers sound like the muffled chattering of "
    "various forest animals calling in the night. Upon closer inspection, green energy flows out of the wand and "
    "seems to curl around its box like an ivy plant trying to crawl into the sunlight.",
};
const vector<string> robe_dialogue = {
    "Upon touching the fabric of one of their robes, it begins to briefly change to a ",
    " hue in your hand. The fabric has an ",
    " appearance and is lined with ",
    " ",
    "."
};

void help(){
    cout << "=============================" << endl;
    cout << "Here is the list of commands available:" << endl;
    cout << "say <message> - sends a message to all users in the space" << endl;
    cout << "inspect <object/area> - allows to inspect items/areas within a given location" << endl;\
    cout << "enter <location> - moves player into a different location" << endl;
    cout << "look - describes the area you are in" << endl;
    cout << "help - lists available commands" << endl;
    cout << "exit - leave the game" << endl;
    cout << "=============================" << endl;
}

vector<string> get_inspects(int loc_id){
    switch(loc_id){
        case VILLAGE:
            return village_inspects;
        case MAGE_STORE:
            return mage_store_inspects;
        case INVALID_LOCATION:
        default:
            return vector<string>{"bolts of light"};
    }
}
vector<string> enter_options(int loc_id){
    switch(loc_id){
        case VILLAGE:
            return village_options;
        case MAGE_STORE:
            return mage_store_options;
        case INVALID_LOCATION:
        default:
            return vector<string>{"village"};
    }
}
void look(int loc_id){
    cout << "=============================" << endl;
    switch(loc_id){
        case VILLAGE:
            cout << village_overview << endl;
            break;
        case MAGE_STORE:
            cout << mage_store_overview << endl;
            break;
        case INVALID_LOCATION:
        default:
            cout << "Somehow you have entered the void. You feel disoriented as you "
            "float in an expanse of gray. Bolts of light whiz by you. Despite never being here"
            "You feel as if letting these bolts touch you could permanantly change and damage you. "
            "Might be best to try and get out of here." << endl;
            break;
    }
    cout << "Here is what you can inspect around you:" << endl;
    vector<string> inspects = get_inspects(loc_id);
    vector<string> options = enter_options(loc_id);
    for(int i = 0;i<inspects.size();i++)
        cout << inspects[i] << endl;
    
    cout << "Here is what locations you can see:" << endl;
    for(int i = 0;i<options.size();i++)
        cout << options[i] << endl;
    cout << "=============================" << endl;
}
void who(const map<int, player_data> &players){
    cout << "=============================" << endl;
    for(const auto& p : players){
        cout << p.second.name << "\t" << get_location(p.second.location) << endl;
    }
    cout << "=============================" << endl;
}

int get_action(string loc){
    if(loc == "say"){
        return SAY_ACTION;
    }else if(loc == "inspect"){
        return INSPECT_ACTION;
    }else if(loc == "enter"){
        return ENTER_ACTION;
    }else if(loc == "look"){
        return LOOK_ACTION;
    }else if(loc == "help"){
        return HELP_ACTION;
    }else if(loc == "who"){
        return WHO_ACTION;
    }
    return INVALID_ACTION;
}

int get_loc_id(string loc){
    if(loc == "village"){
        return VILLAGE;
    }else if(loc == "mage store"){
        return MAGE_STORE;
    }
    return INVALID_LOCATION;
}
string get_location(int loc){
    switch(loc){
        case VILLAGE:
            return "Village";
        case MAGE_STORE:
            return "Mage Store";
        case INVALID_LOCATION:
        default:
            return "The void";
    }
}
#endif