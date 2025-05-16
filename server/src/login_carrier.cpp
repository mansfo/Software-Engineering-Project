#include "main.h"

tuple<string,string,string,string> login_carrier(Con2DB db, vector<string> rv){
    string id = rv.at(5);
    PGresult *res = login_carrier(db, id);
    string name = PQgetvalue(res,0,PQfnumber(res, "name"));
    string surname = PQgetvalue(res,0,PQfnumber(res, "surname"));
    string comp = PQgetvalue(res,0,PQfnumber(res, "company"));
    string status = PQgetvalue(res,0,PQfnumber(res, "status"));
    PQclear(res);
    return tuple<string,string,string,string>(name,surname,comp,status);
}