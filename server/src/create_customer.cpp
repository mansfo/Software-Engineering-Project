#include "main.h"

tuple<string,string,string,string,bool,bool> create_customer(Con2DB db, redisContext* c2r, vector<string> rv){
    string content[6];
    for(int i=5, j=0; i<16 && j<6; i+=2, j++){
        content[j] = rv.at(i);
    }
    string redis_id = rv.at(1);
    if(free_email(db,content[3],"Customer")) return tuple<string,string,string,string,bool,bool>("err","err",redis_id,"err",false,false);
    if(content[4].length()<8) return tuple<string,string,string,string,bool,bool>("err","err",redis_id,"err",true,false);
    PGresult *res = create_customer(db, content[0], content[1], content[2], content[3], content[4], content[5]);
    string id = PQgetvalue(res,0,PQfnumber(res, "id"));
    PQclear(res);

    string readstream = "cust";
    readstream.append(id);
    readstream.append("_read_stream");
    string writestream = "cust";
    writestream.append(id);
    writestream.append("_write_stream");
    redisReply* reply = RedisCommand(c2r, "DEL %s", readstream.c_str());
    assertReply(c2r, reply);
    freeReplyObject(reply);
    reply = RedisCommand(c2r, "DEL %s", writestream.c_str());
    assertReply(c2r, reply);
    freeReplyObject(reply);
    initStreams(c2r, readstream.c_str());
    initStreams(c2r, writestream.c_str());
    add_streams_to_db(db, stoi(id), readstream, writestream, 0);
    return tuple<string,string,string,string,bool,bool>(readstream,writestream,redis_id,id,true,true);
}