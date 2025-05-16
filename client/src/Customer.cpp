#include "Customer.h"

Customer::Customer(string cust_name, string surn, address address, string email, string password, double money){
    redisReply* reply;
    reply = RedisCommand(c2r, "XADD cust0_write_stream * msg new_cust name %s surname %s address %s email %s password %s money %s", cust_name.c_str(), surn.c_str(), addr_to_string(address).c_str(), email.c_str(), password.c_str(), to_string(money).c_str());
    assertReply(c2r, reply);
    string rid = string(reply->str);
    int tries = 0;
    vector<string> rv;
    do{
        if(tries > 50){
            freeReplyObject(reply);
            reply = RedisCommand(c2r, "XADD cust0_write_stream * msg new_cust name %s surname %s address %s email %s password %s money %s", cust_name.c_str(), surn.c_str(), addr_to_string(address).c_str(), email.c_str(), password.c_str(), to_string(money).c_str());
            assertReply(c2r, reply);
            rid = string(reply->str);
            tries = 0;
        }
        tries++;
        freeReplyObject(reply);
        reply = RedisCommand(c2r, "XREAD BLOCK 100 COUNT 1 STREAMS cust0_read_stream %s", add_reply_minus_one(rid).c_str());
        assertReply(c2r,reply);
        if(reply == NULL || reply->type == REDIS_REPLY_NIL) continue;
        rv = getReply(reply);
    } while(reply==NULL || reply->type == REDIS_REPLY_NIL || rv.size() < 3 || strcmp(rid.c_str(),rv.at(3).c_str()));
    if(rv.size()>9 && strcmp(rv.at(5).c_str(),"err")){
        this->id = stoi(rv.at(5));
        this->name = name;
        this->surname = surn;
        this->addr = address;
        this->email = email;
        this->password = password;
        this->read_stream = rv.at(7);
        this->write_stream = rv.at(9);
    }
    else this->cust_ok = false;
}

Customer::Customer(int id){
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
        if(tries > 1000){
            reply = RedisCommand(c2r, "XADD %s * msg login id %d", write_stream.c_str(), id);
            assertReply(c2r,reply);
            rid = string(reply->str);
            freeReplyObject(reply);
            tries = 0;
        }
    } while(infos.size()<1);
    if(infos.size()>2){
        this->name = infos.at(0);
        this->surname = infos.at(1);
        this->addr = str_to_addr(infos.at(2));
    }
    else this->cust_ok = false;
}

Customer::Customer(string email, string password){
    redisReply* reply = RedisCommand(c2r, "XADD cust0_write_stream * msg login email %s password %s", email.c_str(), password.c_str());
    assertReply(c2r, reply);
    string rid = string(reply->str);
    int tries = 0;
    vector<string> rv;
    do{
        if(tries > 50){
            freeReplyObject(reply);
            reply = RedisCommand(c2r, "XADD cust0_write_stream * msg login email %s password %s", email.c_str(), password.c_str());
            assertReply(c2r, reply);
            rid = string(reply->str);
            tries = 0;
        }
        tries++;
        freeReplyObject(reply);
        reply = RedisCommand(c2r, "XREAD BLOCK 100 COUNT 1 STREAMS cust0_read_stream %s", rid.c_str());
        assertReply(c2r,reply);
        rv = getReply(reply);
    } while(reply==NULL || reply->type == REDIS_REPLY_NIL || rv.size() < 4 || strcmp(rid.c_str(),rv.at(3).c_str()));
    if(rv.size()>5 && stoi(rv.at(5))>0) {
        Customer c(stoi(rv.at(5)));
        this->id = c.get_id();
        this->name = c.get_name();
        this->surname = c.get_surname();
        this->addr = c.get_address();
        this->email = email;
        this->password = password;
        this->read_stream = c.get_readstream();
        this->write_stream = c.get_writestream();
    }
    else this->cust_ok = false;
}

address Customer::get_address(){
    return addr;
}

int Customer::get_id(){
    return id;
}

string Customer::get_name(){
    return name;
}

string Customer::get_surname(){
    return surname;
}

string Customer::get_email(){
    return email;
}

string Customer::get_password(){
    return password;
}

string Customer::get_readstream(){
    string readstr = "cust";
    readstr.append(to_string(id));
    readstr.append("_read_stream");
    return readstr;
}

string Customer::get_writestream(){
    string writestr = "cust";
    writestr.append(to_string(id));
    writestr.append("_write_stream");
    return writestr;
}

bool Customer::get_cust_ok(){
    return this->cust_ok;
}