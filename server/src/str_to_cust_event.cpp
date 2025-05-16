#include "main.h"

cust_server_event str_to_cust_event(string str){
    if(!strcmp(str.c_str(),"login")) return cust_server_event::cust_login;
    if(!strcmp(str.c_str(),"buy")) return cust_server_event::buy_something;
    if(!strcmp(str.c_str(),"delete")) return cust_server_event::delete_order;
    if(!strcmp(str.c_str(),"search")) return cust_server_event::searching;
    if(!strcmp(str.c_str(),"history")) return cust_server_event::history;
    if(!strcmp(str.c_str(),"change_addr")) return cust_server_event::cust_change_address;
    if(!strcmp(str.c_str(),"change_psw")) return cust_server_event::cust_change_psw;
    if(!strcmp(str.c_str(),"buy_from_cart")) return cust_server_event::buy_from_cart;
    if(!strcmp(str.c_str(),"to_cart")) return cust_server_event::to_cart;
    if(!strcmp(str.c_str(),"empty_cart")) return cust_server_event::empty_cart;
    if(!strcmp(str.c_str(),"get_cart")) return cust_server_event::get_cart;
    if(!strcmp(str.c_str(),"get_curr_money")) return cust_server_event::get_curr_money;
    if(!strcmp(str.c_str(),"add_money")) return cust_server_event::add_money;
    return cust_server_event::cust_unknown;
}