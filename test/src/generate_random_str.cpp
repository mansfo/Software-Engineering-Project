#include "main.h"

string generate_random_str(int len){
    const string CHARACTERS = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";
    random_device rd;
    mt19937 generator(rd());
    uniform_int_distribution<> distribution(0, CHARACTERS.size() - 1);
    string random_string;
    for (int i = 0; i < len; ++i) {
        random_string
            += CHARACTERS[distribution(generator)];
    }
    return random_string;
}