#include "main.h"
#include "Supplier.h"

vector<pair<int,string>> Supplier::get_products(){
    redisReply* reply = RedisCommand(c2r, "XADD %s * msg all_products suppl %d avail 0", write_stream.c_str(), this->id);
    assertReply(c2r, reply);
    string rid = string(reply->str);
    freeReplyObject(reply);
    int tries = 0, listened = 0;
    do{
        listened = listen_server(rid);
        tries++;
        micro_sleep(1000);
    } while(listened < 1 && tries < 1000);
    return prod_names;
}

vector<pair<int,string>> Supplier::get_products(vector<string> rv){
    vector<pair<int,string>> prods;
    if(rv.size()<6 || strcmp(rv.at(4).c_str(),"products")) return prods;
    string s;
    stringstream ss(rv.at(5));

    while(getline(ss, s, ' ')){
        int id = atoi(s.c_str());
        getline(ss, s, ' ');
        prods.push_back(pair<int,string>(id,s));
    }
    
    return prods;
}
