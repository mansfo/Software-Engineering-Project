#include "main.h"

vector<address> load_addresses(){
    ifstream file("../../test/addresses.txt");
    vector<address> addresses;
    string line;
    while (getline(file, line)) {
        addresses.push_back(str_to_addr(line));
    }

    return addresses;
}