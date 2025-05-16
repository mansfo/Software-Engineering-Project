#include "main.h"
#include "Supplier.h"

vector<pair<int,int>> Supplier::get_products_avail(){
    redisReply* reply;
    reply = RedisCommand(c2r, "XADD %s * msg all_products suppl %d avail 1", write_stream.c_str(), this->id);
    assertReply(c2r, reply);
    string rid = string(reply->str);
    freeReplyObject(reply);
    int tries = 0, listened = 0;
    do{
        listened = listen_server(rid);
        tries++;
        micro_sleep(1000);
    } while(listened < 1 && tries < 1000);
    return prod_avail;
}

vector<pair<int,int>> Supplier::get_products_avail(vector<string> rv){
    vector<pair<int,int>> prods;
    if(rv.size()<6 || strcmp(rv.at(4).c_str(),"products")) return prods;
    string s;
    stringstream ss(rv.at(5));

    while(getline(ss, s, ' ')){
        int id = atoi(s.c_str());
        getline(ss, s, ' ');
        prods.push_back(pair<int,int>(id,atoi(s.c_str())));
    }
    
    return prods;
}