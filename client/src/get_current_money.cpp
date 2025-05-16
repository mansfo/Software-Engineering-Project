#include "main.h"

double Customer::get_current_money(){
    redisReply *reply;
    reply = RedisCommand(c2r, "XADD %s * msg get_curr_money", write_stream.c_str());
    assertReply(c2r, reply);
    string rid = string(reply->str);
    freeReplyObject(reply);
    int tries = 0, listened = 0;
    do{
        listened = listen_server(rid);
        tries++;
        micro_sleep(1000);
    } while(listened == 0 && tries < 1000);
    return curr_money;
}