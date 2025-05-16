#include "main.h"

vector<tuple<int,string,int>> Customer::get_shopping_cart(){
    redisReply *reply;
    shopping_cart.clear();
    reply = RedisCommand(c2r, "XADD %s * msg get_cart", write_stream.c_str());
    assertReply(c2r, reply);
    string rid = string(reply->str);
    freeReplyObject(reply);
    int tries = 0, listened = 0;
    do{
        listened = listen_server(rid);
        tries++;
        micro_sleep(1000);
    } while(listened < 1 && tries < 1000);
    return shopping_cart;
}

vector<tuple<int,string,int>> Customer::get_shopping_cart(vector<string> rv){
    vector<tuple<int,string,int>> cart;
    if(rv.size()<6 || strcmp(rv.at(4).c_str(),"cart") || !strcmp(rv.at(5).c_str(),"err")) return cart;
    string result = rv.at(5);
    string aux;
    stringstream ss(result);
    vector<string> aux_vector;
    while(getline(ss, aux, ' ')){
        aux_vector.push_back(aux);
    }
    for(long unsigned int i=0; i<aux_vector.size()-2; i+=3){
        cart.push_back(tuple<int,string,int>(stoi(aux_vector.at(i)),aux_vector.at(i+1),stoi(aux_vector.at(i+2))));
    }
    return cart;
}