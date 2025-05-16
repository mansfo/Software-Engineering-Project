#include "main.h"

tuple<string,string,string,string> create_carrier(Con2DB db, redisContext* c2r, vector<string> rv){
    string content[3];
    for(int i=5, j=0; i<10 && j<3; i+=2, j++){
        content[j] = rv.at(i);
    }
    string redis_id = rv.at(1);
               
    PGresult *res = create_carrier(db, content[0], content[1], content[2]);
    string id = PQgetvalue(res,0,PQfnumber(res, "id"));
    PQclear(res);

    string readstream = "carr";
    readstream.append(id);
    readstream.append("_read_stream");
    string writestream = "carr";
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
    add_streams_to_db(db, stoi(id), readstream, writestream, 2);
    return tuple<string,string,string,string>(readstream,writestream,redis_id,id);
}