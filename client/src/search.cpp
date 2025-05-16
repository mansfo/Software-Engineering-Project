#include "main.h"

vector<tuple<int,string,int,double>> Customer::search(product_category p_cat){
    search_result.clear();
    redisReply *reply;
    reply = RedisCommand(c2r, "XADD %s * msg search category %s", write_stream.c_str(), cat_to_str(p_cat).c_str());
    assertReply(c2r, reply);
    string rid = string(reply->str);
    freeReplyObject(reply);
    int tries = 0, listened = 0;
    do{
        listened = listen_server(rid);
        tries++;
        micro_sleep(1000);
    } while(listened < 1 && tries < 1000);
    return search_result;
}

vector<tuple<int,string,int,double>> Customer::get_search_result(vector<string> rv){
    vector<tuple<int,string,int,double>> prods = vector<tuple<int,string,int,double>>();
    if(rv.size()<6 || strcmp(rv.at(4).c_str(),"products") || !strcmp(rv.at(5).c_str(),"error")) return prods;
    string result = rv.at(5);
    string aux;
    stringstream ss(result);
    vector<string> aux_vector;
    while(getline(ss, aux, ' ')){
        aux_vector.push_back(aux);
    }
    for(long unsigned int i=0; i<aux_vector.size()-3; i+=4){
        prods.push_back(tuple<int,string,int,double>(stoi(aux_vector.at(i)),aux_vector.at(i+1),stoi(aux_vector.at(i+2)),stod(aux_vector.at(i+3))));
    }
    return prods;
}