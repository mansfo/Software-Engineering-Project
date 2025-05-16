#include "Supplier.h"

Supplier::Supplier(string name, address addr, string email, string password){
    redisReply* reply = RedisCommand(c2r, "XADD suppl0_write_stream * msg new_suppl name %s addr %s email %s password %s", name.c_str(), addr_to_string(addr).c_str(), email.c_str(), password.c_str());
    assertReply(c2r, reply);
    string rid = string(reply->str);
    vector<string> rv;
    int tries = 0;
    do{
        if(tries > 100){
            freeReplyObject(reply);
            reply = RedisCommand(c2r, "XADD suppl0_write_stream * msg new_suppl name %s addr %s email %s password %s", name.c_str(), addr_to_string(addr).c_str(), email.c_str(), password.c_str());
            assertReply(c2r, reply);
            rid = string(reply->str);
            tries = 0;
        }
        tries++;
        freeReplyObject(reply);
        reply = RedisCommand(c2r, "XREAD COUNT 1 BLOCK 1000 STREAMS suppl0_read_stream %s", add_reply_minus_one(rid).c_str());
        assertReply(c2r,reply);
        rv = getReply(reply);
    } while((reply==NULL || reply->type == REDIS_REPLY_NIL || rv.size() < 4 || strcmp(rid.c_str(),rv.at(3).c_str())) && tries <= 50);
    if(rv.size()>9 && strcmp(rv.at(5).c_str(),"err")){
        this->id = stoi(rv.at(5));
        this->name = name;
        this->addr = addr;
        this->email = email;
        this->password = password;
        this->read_stream = rv.at(7);
        this->write_stream = rv.at(9);
    }
    else this->suppl_ok = false;
}

Supplier::Supplier(int id){
    this->id = id;
    this->read_stream = get_readstream();
    this->write_stream = get_writestream();
    redisReply* reply = RedisCommand(c2r, "XADD %s * msg login id %d", write_stream.c_str(), id);
    assertReply(c2r,reply);
    string rid = string(reply->str);
    freeReplyObject(reply);
    int tries = 0;
    vector<string> infos;
    do {
        tries++;
        infos = listen_server(get_readstream(),rid);
        if(tries > 1000){
            reply = RedisCommand(c2r, "XADD %s * msg login id %d", write_stream.c_str(), id);
            assertReply(c2r,reply);
            rid = string(reply->str);
            freeReplyObject(reply);
            tries = 0;
        }
    } while(infos.size()<1);
    if(infos.size()>1){
        this->name = infos.at(0);
        this->addr = str_to_addr(infos.at(1));
    }
}

Supplier::Supplier(string email, string password){
    redisReply* reply = RedisCommand(c2r, "XADD suppl0_write_stream * msg login email %s password %s", email.c_str(), password.c_str());
    assertReply(c2r, reply);
    string rid = string(reply->str);
    vector<string> rv;
    int tries = 0;
    do{
        if(tries > 50){
            freeReplyObject(reply);
            reply = RedisCommand(c2r, "XADD suppl0_write_stream * msg login email %s password %s", email.c_str(), password.c_str());
            assertReply(c2r, reply);
            rid = string(reply->str);
            tries = 0;
        }
        tries++;
        freeReplyObject(reply);
        reply = RedisCommand(c2r, "XREAD BLOCK 100 COUNT 1 STREAMS suppl0_read_stream %s", rid.c_str());
        assertReply(c2r,reply);
        rv = getReply(reply);
    } while((reply==NULL || reply->type == REDIS_REPLY_NIL || rv.size() < 4 || strcmp(rid.c_str(),rv.at(3).c_str())) && tries <= 50);
    if(rv.size() > 5 && stoi(rv.at(5))>0) {
        Supplier s(stoi(rv.at(5)));
        this->id = s.get_id();
        this->name = s.get_name();
        this->addr = s.get_address();
        this->email = email;
        this->password = password;
        this->read_stream = s.get_readstream();
        this->write_stream = s.get_writestream();
    }
    else this->suppl_ok = false;
    micro_sleep(1000);
}

int Supplier::get_id(){
    return id;
}

string Supplier:: get_name(){
    return name;
}

address Supplier:: get_address(){
    return addr;
}

string Supplier::get_password(){
    return password;
}

bool Supplier::get_suppl_ok(){
    return suppl_ok;
}

string Supplier:: get_readstream(){
    string readstr = "suppl";
    readstr.append(std::to_string(id));
    readstr.append("_read_stream");
    return readstr;
}

string Supplier:: get_writestream(){
    string writestr = "suppl";
    writestr.append(std::to_string(id));
    writestr.append("_write_stream");
    return writestr;
}