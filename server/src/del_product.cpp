#include "main.h"

pair<int,int> del_product(Con2DB db, redisReply *r){
    if(r->type == REDIS_REPLY_NIL) return pair<int,int>(0,0);
    vector<string> rv = getReply(r);
    if(rv.size()<8 || strcmp(rv.at(4).c_str(), "suppl") || strcmp(rv.at(6).c_str(), "prod")) return pair<int,int>(0,0);
    string suppl = rv.at(5);
    string prod = rv.at(7);
    return pair<int,int>(stoi(prod),del_product(db, suppl, prod)); 
}