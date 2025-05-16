#include "main.h"

product_category str_to_cat(string s){
    if(!strcmp(s.c_str(),"guitar")) return product_category::guitar;
    if(!strcmp(s.c_str(),"bass")) return product_category::bass;
    if(!strcmp(s.c_str(),"keyboard")) return product_category::keyboard;
    if(!strcmp(s.c_str(),"piano")) return product_category::piano;
    if(!strcmp(s.c_str(),"percussions")) return product_category::percussions;
    if(!strcmp(s.c_str(),"wind")) return product_category::wind_instruments;
    return product_category::other;
}