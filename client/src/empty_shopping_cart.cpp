#include "main.h"

void Customer::empty_shopping_cart(){
    redisReply *reply;
    reply = RedisCommand(c2r, "XADD %s * msg empty_cart",write_stream.c_str());
    assertReply(c2r, reply);
    freeReplyObject(reply);
}