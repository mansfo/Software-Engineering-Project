#include "main.h"

int Carrier::listen_server(string rid){
    redisReply *reply;
    if(rid.length() > 0) reply = RedisCommand(c2r, "XREAD COUNT 1 STREAMS %s %s", read_stream.c_str(), rid.c_str());
    else reply = RedisCommand(c2r, "XREAD COUNT 1 STREAMS %s $", read_stream.c_str());
    assertReply(c2r, reply);
    if(reply == NULL || reply->type == REDIS_REPLY_NIL) return 0;
    vector<string> rv = getReply(reply);
    if(rv.size()>1) last_rid = string(rv.at(1));
    freeReplyObject(reply);
    if(rv.size()>2 && strcmp(rv.at(2).c_str(),"get")){
        int distance;
        if(!strcmp(rv.at(2).c_str(),"deleted_order")){
            current_order_id = 0;
            micro_sleep(50000);
            status = carrier_status::carr_free;
        }
        else if(!strcmp(rv.at(2).c_str(),"msg")){
            if(rv.size()>11 && !strcmp(rv.at(3).c_str(),"login")){
                this->name = rv.at(5);
                this->surname = rv.at(7);
                this->comp = rv.at(9);
                int st = stoi(rv.at(11));
                this->status = st==0? carrier_status::carr_free : (st==1? carrier_status::assigned : (st==2? carrier_status::delivering : carrier_status::coming_back));
            }
            if(rv.size()>7 && !strcmp(rv.at(3).c_str(),"assigned_order")){
                if(stoi(rv.at(5))==-1) status = carrier_status::carr_free;
                if(stoi(rv.at(5))<1) return 0;
                else{
                    update_status();
                    distance = stoi(rv.at(7));
                    micro_sleep(distance*100000);
                    current_order_id = stoi(rv.at(5));
                    update_status();
                    pick_up_order();
                }
            }
            else if(rv.size()>5 && !strcmp(rv.at(3).c_str(),"orders_delivered")){
                if(strcmp(rv.at(5).c_str(),"err") && strcmp(rv.at(5).c_str(),"empty")) deliveries_history = get_orders_delivered(rv);
            }
            else if(rv.size()>5 && !strcmp(rv.at(3).c_str(),"order_picked_up")){
                distance = stoi(rv.at(5));
                micro_sleep(distance*100000);
                update_status();
                deliver_order();
            }
            else if(rv.size()>5 && !strcmp(rv.at(3).c_str(),"delivered")){
                distance = stoi(rv.at(5));
                micro_sleep(distance*100000);
                update_status();
            }
        }
        return 1;
    }
    return 0;
}

vector<string> Carrier::listen_server(string readstream, string redis_id){
    vector<string> res;
    redisReply* reply;
    reply = RedisCommand(c2r, "XREAD COUNT 1 STREAMS %s %s",readstream.c_str(),redis_id.c_str());
    assertReply(c2r,reply);
    if(reply == NULL || reply->type == REDIS_REPLY_NIL) return res;
    vector<string> rv = getReply(reply);
    freeReplyObject(reply);
    if(rv.size()>1) last_rid = rv.at(1);
    if(rv.size()>11){
        if(!strcmp(rv.at(2).c_str(),"msg")){
            if(!strcmp(rv.at(3).c_str(),"login")){
                res.push_back(rv.at(5));
                res.push_back(rv.at(7));
                res.push_back(rv.at(9));
                res.push_back(rv.at(11));
            }
        }
    }
    return res;
}