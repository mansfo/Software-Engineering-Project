#include "main.h"
#include "Supplier.h"

void Supplier::remove_product(int p){
    redisReply *reply;
    reply = RedisCommand(c2r, "XADD %s * msg del_prod suppl %d prod %d", write_stream.c_str(), this->id, p);
    assertReply(c2r, reply);
    string rid = string(reply->str);
    freeReplyObject(reply);
    int tries = 0, listened = 0;
    do{
        listened = listen_server(rid);
        tries++;
        micro_sleep(1000);
    } while(listened==0 && tries < 1000);
}