#include "main.h"

pair<int,string> update_order_status(Con2DB db, Con2DB logdb, redisReply *r){
    vector<string> rv = getReply(r);
    if(rv.size() < 6 || strcmp(rv.at(4).c_str(),"id")) return pair<int,string>(0,"err");
    string id = rv.at(5);
    if(!strcmp(rv.at(3).c_str(),"picked_up")) return pick_up_order(db,logdb,id);
    if(!strcmp(rv.at(3).c_str(),"delivered")) return deliver_order(db,logdb,id);
    return pair<int,string>(0,"err");
}