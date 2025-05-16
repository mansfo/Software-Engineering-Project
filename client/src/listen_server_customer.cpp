#include "main.h"

int Customer::listen_server(string redis_id){
    redisReply *r;
    r = RedisCommand(c2r, "XREAD COUNT 1 STREAMS %s %s", this->read_stream.c_str(), add_reply_minus_one(redis_id).c_str());
    assertReply(c2r,r);
    if(r == nullptr || r->type == REDIS_REPLY_NIL){
        if(r) freeReplyObject(r);
        return 0;
    }
    vector<string> rv = getReply(r);
    if(r) freeReplyObject(r);
    if(rv.size()>3){
        if(!strcmp(rv.at(2).c_str(),"msg")){
            if(rv.size()>9 && !strcmp(rv.at(3).c_str(),"login")){
                this->name = rv.at(5);
                this->surname = rv.at(7);
                this->addr = str_to_addr(rv.at(9));
            }
            if(!strcmp(rv.at(3).c_str(),"orders_history")){
                orders_chronology = get_customer_orders(rv);
            }
            if(!strcmp(rv.at(3).c_str(),"search")){
                search_result = get_search_result(rv);
            }
            if(!strcmp(rv.at(3).c_str(),"get_cart")){
                shopping_cart = get_shopping_cart(rv);
            }
            if(!strcmp(rv.at(3).c_str(),"empty_cart")){
                shopping_cart.clear();
            }
            if(rv.size()>5 && !strcmp(rv.at(3).c_str(),"get_money")){
                curr_money = stod(rv.at(5));
            }
            if(rv.size()>5 && !strcmp(rv.at(3).c_str(),"new_order")){
                return stoi(rv.at(5))>0? stoi(rv.at(5)) : -1;
            }
            if(rv.size()>5 && !strcmp(rv.at(3).c_str(),"new_password")){
                if(!strcmp(rv.at(5).c_str(),"ok")) change_pswd = true;
            }
            return 1;
        }
    }
    return 0;
}

vector<string> Customer::listen_server(string readstream, string redis_id){
    vector<string> res;
    redisReply* reply;
    reply = RedisCommand(c2r, "XREAD COUNT 1 STREAMS %s %s",read_stream.c_str(),redis_id.c_str());
    assertReply(c2r,reply);
    vector<string> rv = getReply(reply);
    if(reply->type != REDIS_REPLY_NIL && rv.size()>9){
        if(!strcmp(rv.at(2).c_str(),"msg")){
            if(!strcmp(rv.at(3).c_str(),"login")){
                res.push_back(rv.at(5));
                res.push_back(rv.at(7));
                res.push_back(rv.at(9));
            }
        }
    }
    freeReplyObject(reply);
    return res;
}