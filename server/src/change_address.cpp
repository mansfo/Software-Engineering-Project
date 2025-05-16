#include "main.h"

int change_address(Con2DB db, redisReply *r, int userid, string usertype){
    if(r->type == REDIS_REPLY_NIL) return 0;
    vector<string> rv = getReply(r);
    if(rv.size()<6 || strcmp(rv.at(4).c_str(),"address")) return 0;
    string addr = rv.at(5);
    return change_address(db, addr, userid, usertype);
}