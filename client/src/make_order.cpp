#include "main.h"
#include "Customer.h"

int Customer::make_order(int prod_id, int quantity){
    redisReply *reply;
    reply = RedisCommand(c2r, "XADD %s * msg buy product %d customer %d quantity %d", write_stream.c_str(), prod_id, id, quantity);
    assertReply(c2r, reply);
    string rid = string(reply->str);
    freeReplyObject(reply);
    int tries = 0, listened = 0;
    do{
        listened = listen_server(rid);
        tries++;
        micro_sleep(1000);
    } while(listened == 0 && tries < 1000);
    return listened;
}