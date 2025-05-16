#include "main.h"

int Supplier::listen_server(string redis_id){
    redisReply *reply;
    reply = RedisCommand(c2r, "XREAD COUNT 1 STREAMS %s %s", read_stream.c_str(),redis_id.c_str());
    assertReply(c2r,reply);
    vector<string> rv = getReply(reply);
    if(reply->type != REDIS_REPLY_NIL && rv.size()>3 && !strcmp(rv.at(2).c_str(),"msg")){
        if(rv.size()>7 && !strcmp(rv.at(3).c_str(),"login")){
            this->name = rv.at(5);
            this->addr = str_to_addr(rv.at(7));
        }
        if(!strcmp(rv.at(3).c_str(),"all_prods")){
            prod_names = get_products(rv);
        }
        if(!strcmp(rv.at(3).c_str(),"all_prods_a")){
            prod_avail = get_products_avail(rv);
        }
        if(rv.size()>7 && !strcmp(rv.at(3).c_str(),"remove_prod")){
            return stoi(rv.at(7));
        }
        if(rv.size()>5 && !strcmp(rv.at(3).c_str(),"new_password")){
            if(!strcmp(rv.at(5).c_str(),"ok")) change_pswd = true;
        }
        freeReplyObject(reply);
        return 1;
    }
    freeReplyObject(reply);
    return 0;
}

vector<string> Supplier::listen_server(string readstream, string redis_id){
    vector<string> res = vector<string>();
    redisReply* reply = RedisCommand(c2r, "XREAD COUNT 1 STREAMS %s %s",read_stream.c_str(),redis_id.c_str());
    assertReply(c2r,reply);
    vector<string> rv = getReply(reply);
    if(reply->type != REDIS_REPLY_NIL && rv.size()>7){
        if(!strcmp(rv.at(2).c_str(),"msg")){
            if(!strcmp(rv.at(3).c_str(),"login")){
                res.push_back(rv.at(5));
                res.push_back(rv.at(7));
            }
        }
    }
    freeReplyObject(reply);
    return res;
}