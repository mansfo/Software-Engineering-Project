#include "main.h"
#include "Supplier.h"

void Supplier::add_product(string name, double price, int avail, product_category cat){
    redisReply *reply;
    reply = RedisCommand(c2r, "XADD %s * msg add_prod name %s price %s avail %d category %s suppl %d", write_stream.c_str(),name.c_str(),to_string(price).c_str(), avail, cat_to_str(cat).c_str(), this->id);
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

void Supplier::add_product(int prod_id, int avail){
    redisReply *reply;
    reply = RedisCommand(c2r, "XADD %s * msg add_prod id %d avail %d suppl %d", write_stream.c_str(), prod_id, avail, this->id);
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