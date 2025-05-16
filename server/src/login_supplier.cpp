#include "main.h"

tuple<string,string> login_supplier(Con2DB db, vector<string> rv){
    string id = rv.at(5);
    PGresult *res = login_supplier(db, id);
    string name = PQgetvalue(res,0,PQfnumber(res, "name"));
    string addr = PQgetvalue(res,0,PQfnumber(res, "addr"));
    PQclear(res);
    return tuple<string,string>(name, addr);
}

pair<string,int> login_with_psw_supplier(Con2DB db, Con2DB logdb, vector<string> rv){
    if(rv.size()<2) return pair<string,int>("err", 0);
    string redis_id = rv.at(1);
    string email, psw;
    if(rv.size() < 8 || strcmp(rv.at(4).c_str(),"email") || strcmp(rv.at(6).c_str(),"password")) return pair<string,int>(redis_id, 0);
    email = rv.at(5);
    psw = rv.at(7);
    return pair<string,int>(redis_id, login_with_psw_supplier(db,logdb,email,psw));
}