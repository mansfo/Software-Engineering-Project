#include "main.h"

suppl_server_event str_to_suppl_event(string str){
    if(!strcmp(str.c_str(), "login")) return suppl_server_event::suppl_login;
    if(!strcmp(str.c_str(), "add_prod")) return suppl_server_event::add_prod;
    if(!strcmp(str.c_str(), "del_prod")) return suppl_server_event::del_prod;
    if(!strcmp(str.c_str(), "all_products")) return suppl_server_event::all_products;
    if(!strcmp(str.c_str(),"change_psw")) return suppl_server_event::suppl_change_psw;
    if(!strcmp(str.c_str(),"set_price")) return suppl_server_event::set_price;
    return suppl_server_event::suppl_unknown;
}