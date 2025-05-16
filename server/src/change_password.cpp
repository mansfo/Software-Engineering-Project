#include "main.h"

int change_password(Con2DB db, redisReply *r, int userid, string usertype){
    if(r->type == REDIS_REPLY_NIL) return 0;
    vector<string> rv = getReply(r);
    if(rv.size()<8 || strcmp(rv.at(4).c_str(),"old") || strcmp(rv.at(6).c_str(),"new")) return 0;
    string old = rv.at(5);
    string new_psw = rv.at(7);
    if(!strcmp(old.c_str(),new_psw.c_str()) || strlen(new_psw.c_str())<8) return 0;
    return change_password(db, old, new_psw, userid, usertype);
}