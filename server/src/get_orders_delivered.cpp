#include "main.h"

string get_orders_delivered(Con2DB db, redisReply *r){
    if(r->type == REDIS_REPLY_NIL) return "err";
    vector<string> rv = getReply(r);
    if(rv.size() < 6 || strcmp(rv.at(4).c_str(), "id")) return "err";
    string carr_id = rv.at(5);
    return get_orders_delivered(db, carr_id);
}