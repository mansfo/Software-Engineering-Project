#include "functions.h"

unordered_map<int,string> get_read_streams(Con2DB db, int table){
    char sqlcmd[500];
    snprintf(sqlcmd,500,"SELECT id,readstream FROM %s WHERE readstream<>'""'",table==0?"Customer":(table==1?"Supplier":"Carrier"));
    PGresult *res = db.ExecSQLtuples(sqlcmd);

    unordered_map<int,string> streams;
    for(int i=0; i<PQntuples(res);i++){
        streams.insert({atoi(PQgetvalue(res,i,PQfnumber(res,"id"))), PQgetvalue(res,i,PQfnumber(res,"readstream"))});
    }
    PQclear(res);
    return streams;
}

unordered_map<int,string> get_write_streams(Con2DB db, int table){
    char sqlcmd[500];
    snprintf(sqlcmd,500,"SELECT id,writestream FROM %s WHERE writestream<>'""'",table==0?"Customer":(table==1?"Supplier":"Carrier"));
    PGresult *res = db.ExecSQLtuples(sqlcmd);

    unordered_map<int,string> streams;
    for(int i=0; i<PQntuples(res);i++){
        streams.insert({atoi(PQgetvalue(res,i,PQfnumber(res,"id"))), PQgetvalue(res,i,PQfnumber(res,"writestream"))});
    }
    PQclear(res);
    return streams;
}