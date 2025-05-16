#include "main.h"

pair<int,string> get_products(Con2DB db, redisReply *r){
    if(r->type == REDIS_REPLY_NIL) return pair<int,string>(-1,"err");
    vector<string> rv = getReply(r);
    if(rv.size()<8 || strcmp(rv.at(4).c_str(),"suppl") || strcmp(rv.at(6).c_str(), "avail")) return pair<int,string>(-1,"err");
    string suppl = rv.at(5);
    int avail = stoi(rv.at(7));
    return pair<int,string>(avail,get_products(db, suppl, avail));
}