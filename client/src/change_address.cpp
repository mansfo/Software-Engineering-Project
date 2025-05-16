#include "main.h"

void Customer::change_address(address addr){
    redisReply *reply;
    reply = RedisCommand(c2r, "XADD %s * msg change_addr address %s", write_stream.c_str(), addr_to_string(addr).c_str());
    assertReply(c2r, reply);
    string rid = string(reply->str);
    freeReplyObject(reply);
    int tries = 0, listened = 0;
    do{
        listened = listen_server(rid);
        tries++;
        micro_sleep(1000);
    } while(listened < 1 && tries < 1000);
    this->addr = addr;
}