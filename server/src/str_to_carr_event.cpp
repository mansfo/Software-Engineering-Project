#include "main.h"

carr_server_event str_to_carr_event(string str){
    if(!strcmp(str.c_str(), "login")) return carr_server_event::carr_login;
    if(!strcmp(str.c_str(), "picked_up")) return carr_server_event::picked_up;
    if(!strcmp(str.c_str(), "delivered")) return carr_server_event::delivered;
    if(!strcmp(str.c_str(),"orders_delivered")) return carr_server_event::orders_delivered;
    if(!strcmp(str.c_str(),"order_assigned")) return carr_server_event::order_assigned;
    return carr_server_event::carr_unknown;
}