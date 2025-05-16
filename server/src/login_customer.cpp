#include "main.h"

tuple<string,string,string> login_customer(Con2DB db, redisReply *r){
    if(r->type == REDIS_REPLY_NIL) return tuple<string,string,string>("err","err","err");
    vector<string> rv = getReply(r);
    if(rv.size() < 4 || strcmp(rv.at(4).c_str(), "id")) return tuple<string,string,string>("err","err","err");
    string id = r->element[0]->element[1]->element[0]->element[1]->element[3]->str;
    PGresult *res = login_customer(db, id);
    string name = PQgetvalue(res,0,PQfnumber(res, "name"));
    string surname = PQgetvalue(res,0,PQfnumber(res, "surname"));
    string addr = PQgetvalue(res,0,PQfnumber(res, "addr"));
    PQclear(res);
    return tuple<string,string,string>(name,surname,addr);
}

tuple<string,int,string,string> login_with_psw_customer(Con2DB db, Con2DB logdb, redisContext* c2r, vector<string> rv){
    if(rv.size()<=1) return tuple<string,int,string,string>("err", 0, "err", "err");
    string redis_id = rv.at(1);
    string email, psw;
    if(rv.size() < 8 || strcmp(rv.at(4).c_str(),"email") || strcmp(rv.at(6).c_str(),"password")) return tuple<string,int,string,string>(redis_id, 0, "err", "err");
    email = rv.at(5);
    psw = rv.at(7);
    int id = login_with_psw_customer(db,logdb,email,psw);
    string readstream = "cust";
    readstream.append(to_string(id));
    readstream.append("_read_stream");
    string writestream = "cust";
    writestream.append(to_string(id));
    writestream.append("_write_stream");
    redisReply* reply = RedisCommand(c2r, "DEL %s", readstream.c_str());
    assertReply(c2r, reply);
    freeReplyObject(reply);
    reply = RedisCommand(c2r, "DEL %s", writestream.c_str());
    assertReply(c2r, reply);
    freeReplyObject(reply);
    initStreams(c2r, readstream.c_str());
    initStreams(c2r, writestream.c_str());
    return tuple<string,int,string,string>(redis_id, id, readstream, writestream);
}