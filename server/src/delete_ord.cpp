#include "main.h"

int delete_ord(Con2DB db, redisContext* c2r, redisReply *r){
    if(r->type == REDIS_REPLY_NIL) return 0;
    vector<string> rv = getReply(r);
    if(rv.size()<8 || strcmp(rv.at(4).c_str(), "order") || strcmp(rv.at(6).c_str(), "cust")) return 0;
    string ord = rv.at(5);
    string cust = rv.at(7);
    string rid = rv.at(1);
    redisReply *reply_suppl;
    reply_suppl = RedisCommand(c2r, "XADD %s * msg del_order redis_id %s order %s cust %s", CUST_WRITE_SUPPL, rid.c_str(), ord.c_str(), cust.c_str());
    assertReply(c2r, reply_suppl);
    string rid_s = reply_suppl->str;
    int tries = 0;
    do{
        if(tries > 1000){
            freeReplyObject(reply_suppl);  
            reply_suppl = RedisCommand(c2r, "XADD %s * msg del_order redis_id %s order %s cust %s", CUST_WRITE_SUPPL, rid.c_str(), ord.c_str(), cust.c_str());
            assertReply(c2r, reply_suppl);
            rid_s = reply_suppl->str;
            tries = 0;
        }
        freeReplyObject(reply_suppl);
        reply_suppl = RedisCommand(c2r, "XREAD COUNT 1 BLOCK 10 STREAMS %s %s", CUST_READ_SUPPL, rid_s.c_str());
        assertReply(c2r, reply_suppl);
        tries++;
        rv = getReply(reply_suppl);
        micro_sleep(1000);
    } while(rv.size()<6 || strcmp(rv.at(3).c_str(),rid.c_str()));
    if(strcmp(rv.at(4).c_str(),"status") || strcmp(rv.at(5).c_str(),"ok")) return 0;
    freeReplyObject(reply_suppl);
    return 1;
}