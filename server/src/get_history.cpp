#include "main.h"

string get_history(Con2DB db, redisReply *r){
    if(r->type == REDIS_REPLY_NIL) return "err";
    vector<string> rv = getReply(r);
    if(rv.size() < 6 || strcmp(rv.at(4).c_str(), "id")) return "err";
    return get_history(db, rv.at(5));
}