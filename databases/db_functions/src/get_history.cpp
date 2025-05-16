#include "functions.h"

string get_history(Con2DB db, string cust){
    char sqlcmd[500];
    snprintf(sqlcmd, 500, "SELECT o.id id, p.name name FROM Orders o, Product p WHERE p.id = o.prod AND o.cust = '%s' ORDER BY o.id DESC", cust.c_str());
    PGresult *res = db.ExecSQLtuples(sqlcmd);
    int rows = PQntuples(res);
    string result = "";
    for(int i=0; i<rows; i++){
        if(i!=0) result.append(" ");
        result.append(PQgetvalue(res,i,PQfnumber(res,"id")));
        result.append(" ");
        result.append(PQgetvalue(res,i,PQfnumber(res,"name")));
    }
    return result;
}