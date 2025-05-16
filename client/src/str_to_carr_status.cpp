#include "main.h"

carrier_status str_to_carr_status(string str){
    int s = stoi(str);
    if(s==0) return carrier_status::carr_free;
    if(s==1) return carrier_status::assigned;
    if(s==2) return carrier_status::delivering;
    return carrier_status::coming_back;
}