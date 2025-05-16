#include "Carrier.h"

int Carrier::get_order_assigned(){
    redisReply *reply = RedisCommand(c2r, "XADD %s * msg order_assigned id %d", write_stream.c_str(), this->id);
    assertReply(c2r, reply);
    string rid = string(reply->str);
    freeReplyObject(reply);
    int tries = 0, listened = 0;
    do{
        listened = listen_server(rid);
        tries++;
        micro_sleep(1000);
    } while(listened < 1 && tries < 100);
    return listened;
}