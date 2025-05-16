#include "main.h"

void Customer::add_money(double m){
    redisReply *reply;
    reply = RedisCommand(c2r, "XADD %s * msg add_money money %s", write_stream.c_str(), to_string(m).c_str());
    assertReply(c2r, reply);
    freeReplyObject(reply);
}