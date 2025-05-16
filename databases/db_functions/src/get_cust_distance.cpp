#include "functions.h"

int get_cust_distance(Con2DB db, int order){
    PGresult *res;
    char sqlcmd[200];
    snprintf(sqlcmd,200,"SELECT s.addr as a1, c.addr as a2 FROM Customer c, Orders o, Supplier s WHERE o.id = '%d' AND o.suppl = s.id AND o.cust = c.id", order);
    res = db.ExecSQLtuples(sqlcmd);
    string suppl_addr = PQgetvalue(res,0,PQfnumber(res,"a1"));
    string cust_addr = PQgetvalue(res,0,PQfnumber(res,"a2"));
    vector<string> suppl;
    vector<string> cust;
    stringstream ss(suppl_addr);
    string aux;
    while(getline(ss,aux,',')){
        suppl.push_back(aux);
    }
    stringstream ss2(cust_addr);
    while(getline(ss2,aux,',')){
        cust.push_back(aux);
    }
    if(cust.size()<5 || suppl.size() < 5) return 1;
    if(!strcmp(suppl.at(2).c_str(),cust.at(2).c_str())) return 1;
    if(!strcmp(suppl.at(3).c_str(),cust.at(3).c_str())) return 2;
    if(!strcmp(suppl.at(4).c_str(),cust.at(4).c_str())) return 3;
    return 4;
}