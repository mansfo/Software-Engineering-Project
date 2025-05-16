#include "main.h"

order_status str_to_status(string str){
    if(!strcmp(str.c_str(),"not sent")) return order_status::not_sent;
    if(!strcmp(str.c_str(),"sent")) return order_status::sent;
    if(!strcmp(str.c_str(),"arrived")) return order_status::arrived;
    if(!strcmp(str.c_str(),"deleted")) return order_status::deleted;
    if(!strcmp(str.c_str(),"deleting_ok")) return order_status::deleting_ok;
    return order_status::not_found;
}