#include "main.h"
#include "Carrier.h"

vector<pair<int,string>> Carrier::get_orders_delivered(){
    redisReply *reply = RedisCommand(c2r, "XADD %s * msg orders_delivered id %d", write_stream.c_str(), this->id);
    assertReply(c2r, reply);
    string rid = string(reply->str);
    freeReplyObject(reply);
    int tries = 0, listened = 0;
    do{
        listened = listen_server(rid);
        tries++;
        micro_sleep(1000);
    } while(listened < 1 && tries < 1000);
    return deliveries_history;
}

vector<pair<int,string>> Carrier::get_orders_delivered(vector<string> rv){
    vector<pair<int,string>> prods;
    if(rv.size()<=5) return prods;
    string s;
    stringstream ss(rv.at(5));

    while(getline(ss, s, ' ')){
        int id = stoi(s);
        getline(ss, s, ' ');
        prods.push_back(pair<int,string>(id,s));
    }
    
    return prods;
}