#include "main.h"

int add_product(Con2DB db, redisReply *r){
    if(r->type == REDIS_REPLY_NIL) return 0;
    vector<string> rv = getReply(r);
    if(rv.size()<14 || strcmp(rv.at(4).c_str(), "name") || strcmp(rv.at(6).c_str(), "price") || strcmp(rv.at(8).c_str(), "avail") || strcmp(rv.at(10).c_str(), "category") || strcmp(rv.at(12).c_str(), "suppl")) return 0;
    string name = rv.at(5);
    string price = rv.at(7);
    string avail = rv.at(9);
    string cat = rv.at(11);
    string id = rv.at(13);
    return add_product(db, name, price, avail, cat, id);
}

int add_product(Con2DB db, vector<string> rv){
    if(rv.size()<10 || strcmp(rv.at(6).c_str(), "avail") || strcmp(rv.at(8).c_str(), "suppl")) return 0;
    string prod_id = rv.at(5);
    string avail = rv.at(7);
    string suppl = rv.at(9);
    return add_product(db, prod_id, avail, suppl);
}