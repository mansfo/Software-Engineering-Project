#include "main.h"

void Customer::add_to_cart(int product, string name, int quantity){
    redisReply *reply;
    reply = RedisCommand(c2r, "XADD %s * msg to_cart product %d name %s quantity %d", write_stream.c_str(), product, name.c_str(), quantity);
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