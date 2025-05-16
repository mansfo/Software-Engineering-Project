#include "main.h"

void Customer::change_password(string old, string new_psw){
    redisReply *reply;
    reply = RedisCommand(c2r, "XADD %s * msg change_psw old %s new %s", write_stream.c_str(), old.c_str(), new_psw.c_str());
    assertReply(c2r,reply);
    string rid = string(reply->str);
    freeReplyObject(reply);
    int tries = 0, listened = 0;
    do{
        listened = listen_server(rid);
        tries++;
        micro_sleep(1000);
    } while(listened < 1 && tries < 1000);
    if(change_pswd) password = new_psw;
    change_pswd = false;
}

void Supplier::change_password(string old, string new_psw){
    redisReply *reply;
    reply = RedisCommand(c2r, "XADD %s * msg change_psw old %s new %s", write_stream.c_str(), old.c_str(), new_psw.c_str());
    assertReply(c2r,reply);
    string rid = string(reply->str);
    freeReplyObject(reply);
    int tries = 0, listened = 0;
    do{
        listened = listen_server(rid);
        tries++;
        micro_sleep(1000);
    } while(listened < 1 && tries < 1000);
    if(change_pswd) password = new_psw;
    change_pswd = false;
}