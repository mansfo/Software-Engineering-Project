#include "main.h"
#include "Carrier.h"

void Carrier::deliver_order(){
    redisReply* reply = RedisCommand(c2r, "XADD %s * msg delivered id %d", write_stream.c_str(), current_order_id);
    assertReply(c2r, reply);
    string rid = string(reply->str);
    freeReplyObject(reply);
    int tries = 0, listened = 0;
    do{
        listened = listen_server(rid);
        tries++;
        micro_sleep(1000);
    } while(listened < 1 && tries < 1000);
}