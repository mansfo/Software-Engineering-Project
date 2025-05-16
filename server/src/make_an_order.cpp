#include "main.h"

int make_an_order(Con2DB db, redisContext* c2r, redisReply *r){
    if(r->type == REDIS_REPLY_NIL) return -1;
    vector<string> rv = getReply(r);
    if(rv.size()<10 || strcmp(rv.at(4).c_str(), "product") || strcmp(rv.at(6).c_str(), "customer") || strcmp(rv.at(8).c_str(), "quantity")) return -2;
    string prod = rv.at(5);
    string cust = rv.at(7);
    string quant = rv.at(9);
    string rid = rv.at(1);
    redisReply *reply_suppl;
    reply_suppl = RedisCommand(c2r, "XADD %s * msg new_order redis_id %s product %s cust %s quantity %s", CUST_WRITE_SUPPL, rid.c_str(), prod.c_str(), cust.c_str(), quant.c_str());
    assertReply(c2r, reply_suppl);
    string rid_s = reply_suppl->str;
    int tries = 0;
    do{
        if(tries > 1000){
            freeReplyObject(reply_suppl);  
            reply_suppl = RedisCommand(c2r, "XADD %s * msg new_order redis_id %s product %s cust %s quantity %s", CUST_WRITE_SUPPL, rid.c_str(), prod.c_str(), cust.c_str(), quant.c_str());
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
    } while(rv.size()<8 || strcmp(rv.at(3).c_str(),rid.c_str()));
    if(strcmp(rv.at(6).c_str(),"status") || strcmp(rv.at(7).c_str(),"ok") || strcmp(rv.at(4).c_str(),"order_id")) return -3;
    string ord_id = rv.at(5);
    freeReplyObject(reply_suppl);
    return stoi(ord_id);
}