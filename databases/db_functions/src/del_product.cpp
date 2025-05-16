#include "functions.h"

int del_product(Con2DB db, string suppl, string prod){
    char sqlcmd[500];
    PGresult *res;
    snprintf(sqlcmd,500,"SELECT avail FROM Product WHERE supplier = '%s' AND id = '%s'",suppl.c_str(), prod.c_str());
    res = db.ExecSQLtuples(sqlcmd);
    if(PQntuples(res)<1 || stoi(PQgetvalue(res,0,PQfnumber(res,"avail")))>0) return -1;
    snprintf(sqlcmd, 500, "UPDATE Product SET removed = 1 WHERE id = '%s'", prod.c_str());
    res = db.ExecSQLcmd(sqlcmd);
    PQclear(res);
    return stoi(prod);
}