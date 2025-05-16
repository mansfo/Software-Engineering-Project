#include "main.h"

void Supplier::set_price(int prod_id, double price){
    redisReply *reply;
    reply = RedisCommand(c2r, "XADD %s * msg set_price id %d price %s suppl %d", write_stream.c_str(), prod_id, to_string(price).c_str(), this->id);
    assertReply(c2r, reply);
    string rid = string(reply->str);
    freeReplyObject(reply);
    int tries = 0, listened = 0;
    do{
        listened = listen_server(rid);
        tries++;
        micro_sleep(1000);
    } while(listened<1 && tries < 1000);
}