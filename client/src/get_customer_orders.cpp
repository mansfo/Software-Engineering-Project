#include "main.h"
#include "Customer.h"

vector<pair<int,string>> Customer::get_customer_orders(){
    orders_chronology.clear();
    redisReply *reply = RedisCommand(c2r, "XADD %s * msg history id %d", write_stream.c_str(), id);
    assertReply(c2r, reply); 
    string rid = string(reply->str);
    freeReplyObject(reply);
    int tries = 0, listened = 0;
    do{
        listened = listen_server(rid);
        tries++;
        micro_sleep(1000);
    } while(listened < 1 && tries < 1000);
    return orders_chronology;
}

vector<pair<int,string>> Customer::get_customer_orders(vector<string> rv){
    vector<pair<int,string>> orders;
    if(rv.size()<6 || strcmp(rv.at(4).c_str(),"orders")) return orders;
    string result = rv.at(5);
    string aux;
    stringstream ss(result);
    vector<string> aux_vector;
    while(getline(ss, aux, ' ')){
        aux_vector.push_back(aux);
    }
    for(long unsigned int i=0; i<aux_vector.size(); i+=2){
        orders.push_back(pair<int,string>(stoi(aux_vector.at(i)),aux_vector.at(i+1)));
    }
    return orders;
}