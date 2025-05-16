#include "Carrier.h"

Carrier::Carrier(string name, string surname, string comp){
    redisReply* reply = RedisCommand(c2r, "XADD carr0_write_stream * msg new_carr name %s surname %s comp %s", name.c_str(), surname.c_str(), comp.c_str());
    assertReply(c2r, reply);
    string rid = string(reply->str);
    int tries = 0;
    vector<string> rv;
    do{
        if(tries > 1000){
            freeReplyObject(reply);
            reply = RedisCommand(c2r, "XADD carr0_write_stream * msg new_carr name %s surname %s comp %s", name.c_str(), surname.c_str(), comp.c_str());
            assertReply(c2r, reply);
            rid = string(reply->str);
            tries = 0;
        }
        tries++;
        freeReplyObject(reply);
        reply = RedisCommand(c2r, "XREAD COUNT 1 BLOCK 1000 STREAMS carr0_read_stream %s", rid.c_str());
        assertReply(c2r,reply);
        rv = getReply(reply);
    } while(reply==NULL || reply->type == REDIS_REPLY_NIL || rv.size() < 3 || strcmp(rid.c_str(),rv.at(3).c_str()));
    if(rv.size()>5 && stoi(rv.at(5))>0){
        this->id = stoi(rv.at(5));
        this->name = name;
        this->surname = surname;
        this->comp = comp;
        this->status = carrier_status::carr_free;
    }
}

Carrier::Carrier(int id){
    this->id = id;
    this->read_stream = get_readstream();
    this->write_stream = get_writestream();
    redisReply* reply;
    reply = RedisCommand(c2r, "XADD %s * msg login id %d", write_stream.c_str(), id);
    assertReply(c2r,reply);
    string rid = string(reply->str);
    freeReplyObject(reply);
    int tries = 0;
    vector<string> infos;
    do {
        tries++;
        infos = listen_server(get_readstream(),rid);
        if(tries > 10000){
            reply = RedisCommand(c2r, "XADD %s * msg login id %d", write_stream.c_str(), id);
            assertReply(c2r,reply);
            rid = string(reply->str);
            freeReplyObject(reply);
            tries = 0;
        }
        micro_sleep(1000);
    } while(infos.size()<1);
    if(infos.size()>3){
        this->name = infos.at(0);
        this->surname = infos.at(1);
        this->comp = infos.at(2);
        this->status = str_to_carr_status(infos.at(3));
    }
}

int Carrier::get_id(){
    return id;
}

string Carrier::get_name(){
    return name;
}

string Carrier::get_surname(){
    return surname;
}

string Carrier::get_company(){
    return this->comp;
}

string Carrier::get_last_rid(){
    if(last_rid.length()>0) return last_rid;
    redisReply *reply = RedisCommand(c2r, "XADD aux_stream * get redis_id");
    assertReply(c2r,reply);
    last_rid = string(reply->str);
    freeReplyObject(reply);
    return last_rid;
}

string Carrier::get_readstream(){
    string readstr = "carr";
    readstr.append(to_string(id));
    readstr.append("_read_stream");
    return readstr;
}

string Carrier::get_writestream(){
    string writestr = "carr";
    writestr.append(to_string(id));
    writestr.append("_write_stream");
    return writestr;
}

int Carrier::get_current_order_id(){
    return current_order_id;
}

void Carrier::set_current_order_id(int ord_id){
    current_order_id = ord_id;
}

void Carrier::update_status(){
    if(status == carrier_status::carr_free) status = carrier_status::assigned;
    if(status == carrier_status::assigned) status = carrier_status::delivering;
    if(status == carrier_status::delivering) status = carrier_status::coming_back;
    if(status == carrier_status::coming_back) status = carrier_status::carr_free;
}

carrier_status Carrier::get_status(){
    return status;
}