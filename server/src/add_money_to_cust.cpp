#include "main.h"

int add_money_to_cust(Con2DB db, redisReply *r, int userid){
    if(r->type == REDIS_REPLY_NIL) return 0;
    vector<string> rv = getReply(r);
    if(rv.size()<6 || strcmp(rv.at(4).c_str(),"money")) return 0;
    string money = rv.at(5);
    return add_money_to_cust(db, money, userid);
}