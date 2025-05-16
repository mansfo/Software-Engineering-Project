#include "main.h"
#include "Customer.h"

void Customer::delete_order(int order_id){
    redisReply *reply;
    reply = RedisCommand(c2r, "XADD %s * msg delete order %d cust %d", write_stream.c_str(), order_id, this->id);
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