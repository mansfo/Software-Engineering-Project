#include "main.h"

tuple<string,string,string,string> create_supplier(Con2DB db, redisContext* c2r, vector<string> rv){
    string content[4];
    for(int i=5, j=0; i<12 && j<4; i+=2, j++){
        content[j] = rv.at(i);
    }
    string redis_id = rv.at(1);
    if(free_email(db,content[2],"Supplier")) return tuple<string,string,string,string>("err","err",redis_id,"err");
    if(content[3].length()<8) return tuple<string,string,string,string>("err","err",redis_id,"err");
    PGresult *res = create_supplier(db, content[0], content[1], content[2], content[3]);
    string id = PQgetvalue(res,0,PQfnumber(res, "id"));
    PQclear(res);

    string readstream = "suppl";
    readstream.append(id);
    readstream.append("_read_stream");
    string writestream = "suppl";
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
    add_streams_to_db(db, stoi(id), readstream.c_str(), writestream.c_str(), 1);
    return tuple<string,string,string,string>(readstream,writestream,redis_id,id);
}