#include "functions.h"

int get_suppl_distance(Con2DB db, int order){
    PGresult *res;
    char sqlcmd[200];
    snprintf(sqlcmd,200,"SELECT s.addr as a1, co.addr as a2 FROM Carrier c, Company co, Orders o, Supplier s WHERE o.id = '%d' AND o.suppl = s.id AND o.carr = c.id AND c.company = co.name", order);
    res = db.ExecSQLtuples(sqlcmd);
    string suppl_addr = PQgetvalue(res,0,PQfnumber(res,"a1"));
    string carr_addr = PQgetvalue(res,0,PQfnumber(res,"a2"));
    vector<string> suppl;
    vector<string> carr;
    stringstream ss(suppl_addr);
    string aux;
    while(getline(ss,aux,',')){
        suppl.push_back(aux);
    }
    stringstream ss2(carr_addr);
    while(getline(ss2,aux,',')){
        carr.push_back(aux);
    }
    if(suppl.size()<5 || carr.size() < 5) return 1;
    if(!strcmp(suppl.at(2).c_str(),carr.at(2).c_str())) return 1;
    if(!strcmp(suppl.at(3).c_str(),carr.at(3).c_str())) return 2;
    if(!strcmp(suppl.at(4).c_str(),carr.at(4).c_str())) return 3;
    return 4;
}